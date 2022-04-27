#include <agrpc/asioGrpc.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <hello_agrpc/hello_agrpc.grpc.pb.h>

int main(int argc, char** argv)
{
    const std::string host = "0.0.0.0";
    const std::string port = argc >= 2 ? argv[1] : "50051";
    const std::string addr = host + ":" + port;

    std::unique_ptr<grpc::Server> server;
    grpc::ServerBuilder builder;
    agrpc::GrpcContext grpc_context{builder.AddCompletionQueue()};
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    hello_agrpc::Greeter::AsyncService service;
    builder.RegisterService(&service);
    server = builder.BuildAndStart();

    boost::asio::co_spawn(
        grpc_context,
        [&]() -> boost::asio::awaitable<void>
        {
            grpc::ServerContext server_context;
            hello_agrpc::HelloRequest request;
            grpc::ServerAsyncResponseWriter<hello_agrpc::HelloReply> writer{&server_context};
            co_await agrpc::request(&hello_agrpc::Greeter::AsyncService::RequestSayHello, service, server_context, request, writer);
            hello_agrpc::HelloReply response;
            response.set_message("Hello, " + request.name() + "!");
            co_await agrpc::finish(writer, response, grpc::Status::OK);
        },
        boost::asio::detached);

    grpc_context.run();
    server->Shutdown();
}
