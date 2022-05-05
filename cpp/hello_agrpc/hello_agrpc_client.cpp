#include <agrpc/asioGrpc.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <hello_agrpc/hello_agrpc.grpc.pb.h>

int main(int argc, char** argv)
{
    const std::string host = "localhost";
    const std::string port = argc >= 2 ? argv[1] : "50051";
    //const std::string addr = host + ":" + port;
    const std::string addr = "unix:///tmp/test.sock";

    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());
    std::unique_ptr<hello_agrpc::Greeter::Stub> stub = hello_agrpc::Greeter::NewStub(channel);
    agrpc::GrpcContext grpc_context{std::make_unique<grpc::CompletionQueue>()};

    grpc::Status status;
    boost::asio::co_spawn(
        grpc_context,
        [&]() -> boost::asio::awaitable<void>
        {
            grpc::ClientContext client_context;
            hello_agrpc::HelloRequest request;
            request.set_name("World");
            std::unique_ptr<grpc::ClientAsyncResponseReader<hello_agrpc::HelloReply>> reader =
                stub->AsyncSayHello(&client_context, request, agrpc::get_completion_queue(grpc_context));
            hello_agrpc::HelloReply response;
            co_await agrpc::finish(*reader, response, status);
            std::cout << response.message() << std::endl;
        },
        boost::asio::detached);

    grpc_context.run();
    if (!status.ok())
    {
        std::cerr << "\033[38;5;196m" << status.error_message() << "\033[m" << std::endl;
        std::abort();
    }
}
