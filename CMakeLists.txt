cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(hello_agrpc VERSION 1.0.0.0 LANGUAGES C CXX)

find_package(Boost COMPONENTS filesystem system REQUIRED CONFIG)

find_package(grpc REQUIRED)
find_package(asio-grpc REQUIRED CONFIG)

asio_grpc_protobuf_generate(
    GENERATE_GRPC
    OUT_VAR "ASIO_GRPC_EXAMPLE_PROTO_SOURCES"
    OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/hello_agrpc"
    IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/proto/hello_agrpc"
    PROTOS "${CMAKE_CURRENT_LIST_DIR}/proto/hello_agrpc/hello_agrpc.proto")

add_executable(hello_agrpc_server cpp/hello_agrpc/hello_agrpc_server.cpp ${ASIO_GRPC_EXAMPLE_PROTO_SOURCES})
target_include_directories(hello_agrpc_server PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_link_libraries(hello_agrpc_server PRIVATE asio-grpc::asio-grpc)
target_compile_features(hello_agrpc_server PRIVATE cxx_std_20)

add_executable(hello_agrpc_client cpp/hello_agrpc/hello_agrpc_client.cpp ${ASIO_GRPC_EXAMPLE_PROTO_SOURCES})
target_include_directories(hello_agrpc_client PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_link_libraries(hello_agrpc_client PRIVATE asio-grpc::asio-grpc)
target_compile_features(hello_agrpc_client PRIVATE cxx_std_20)

add_executable(hello cpp/hello_agrpc/hello.cpp)
target_link_libraries(hello PRIVATE Boost::filesystem Boost::system dl)
target_compile_features(hello PRIVATE cxx_std_20)
