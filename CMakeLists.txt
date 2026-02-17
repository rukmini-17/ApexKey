cmake_minimum_required(VERSION 3.15)
project(ApexKey LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# 1. Find the required packages
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# 2. Path to proto file
set(PROTO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proto/kv_store.proto")
set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")

# 3. Generate C++ files from the .proto file
# uses gRPC and Protobuf targets to handle generation
get_target_property(grpc_cpp_plugin gRPC::grpc_cpp_plugin LOCATION)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/kv_store.pb.cc"
           "${CMAKE_CURRENT_BINARY_DIR}/kv_store.pb.h"
           "${CMAKE_CURRENT_BINARY_DIR}/kv_store.grpc.pb.cc"
           "${CMAKE_CURRENT_BINARY_DIR}/kv_store.grpc.pb.h"
    COMMAND protobuf::protoc
    ARGS --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --plugin=protoc-gen-grpc="${grpc_cpp_plugin}"
         -I "${PROTO_PATH}"
         "${PROTO_FILE}"
    DEPENDS "${PROTO_FILE}"
)

# 4. Define Server Executable
add_executable(ApexKeyServer 
    src/server.cpp 
    "${CMAKE_CURRENT_BINARY_DIR}/kv_store.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/kv_store.grpc.pb.cc"
)

# 5. Link everything together
target_link_libraries(ApexKeyServer 
    PRIVATE 
    gRPC::grpc++ 
    protobuf::libprotobuf
    Threads::Threads
)

# 6. Tell the compiler where to find the generated headers
target_include_directories(ApexKeyServer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(ApexKeyClient src/client.cpp "${CMAKE_CURRENT_BINARY_DIR}/kv_store.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/kv_store.grpc.pb.cc")
target_link_libraries(ApexKeyClient PRIVATE gRPC::grpc++ protobuf::libprotobuf Threads::Threads)
target_include_directories(ApexKeyClient PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")