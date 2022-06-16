// Copyright 2022 Dennis Hezel
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <hello_agrpc/hello_agrpc.grpc.pb.h>

void abort_if_not(bool condition)
{
    if (!condition)
    {
        std::abort();
    }
}

template <class... Args>
void silence_unused(Args&&... args)
{
    ((void)args, ...);
}

#include <agrpc/asioGrpc.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <forward_list>
#include <thread>
#include <vector>

namespace asio = boost::asio;

// A simple round robin strategy for picking the next GrpcContext to use for an RPC.
template <class Iterator>
class RoundRobin
{
  public:
    RoundRobin(Iterator begin, std::size_t size) : begin(begin), size(size) {}

    decltype(auto) next()
    {
        const auto cur = current.fetch_add(1, std::memory_order_relaxed);
        const auto pos = cur % size;
        return *std::next(begin, pos);
    }

  private:
    Iterator begin;
    std::size_t size;
    std::atomic_size_t current{};
};

asio::awaitable<void> make_request(agrpc::GrpcContext& grpc_context, hello_agrpc::Greeter::Stub& stub)
{
    grpc::ClientContext client_context;
    hello_agrpc::HelloRequest request;
    request.set_name("world");
    const auto reader =
        agrpc::request(&hello_agrpc::Greeter::Stub::AsyncSayHello, stub, client_context, request, grpc_context);
    hello_agrpc::HelloReply response;
    grpc::Status status;
    co_await agrpc::finish(reader, response, status);

    abort_if_not(status.ok());
}

int main(int argc, const char** argv)
{
    const auto port = argc >= 2 ? argv[1] : "50051";
    const auto host = std::string("localhost:") + port;
    const auto thread_count = std::thread::hardware_concurrency();

    hello_agrpc::Greeter::Stub stub{grpc::CreateChannel(host, grpc::InsecureChannelCredentials())};
    std::forward_list<agrpc::GrpcContext> grpc_contexts;
    std::vector<asio::executor_work_guard<agrpc::GrpcContext::executor_type>> guards;

    // Create GrpcContexts and their work guards.
    for (size_t i = 0; i < thread_count; ++i)
    {
        auto& grpc_context = grpc_contexts.emplace_front(std::make_unique<grpc::CompletionQueue>());
        guards.emplace_back(grpc_context.get_executor());
    }

    // Create one thread per GrpcContext.
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (size_t i = 0; i < thread_count; ++i)
    {
        threads.emplace_back(
            [&, i]
            {
                auto& grpc_context = *std::next(grpc_contexts.begin(), i);
                grpc_context.run();
            });
    }

    // Make requests.
    RoundRobin round_robin_grpc_contexts{grpc_contexts.begin(), thread_count};
    for (size_t i{}; i < 20; ++i)
    {
        auto& grpc_context = round_robin_grpc_contexts.next();
        boost::asio::co_spawn(grpc_context, make_request(grpc_context, stub), boost::asio::detached);
    }

    guards.clear();

    for (auto& thread : threads)
    {
        thread.join();
    }
}
