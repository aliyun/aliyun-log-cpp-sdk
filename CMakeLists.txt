cmake_minimum_required(VERSION 2.6)
project(sls-sdk-cpp)

find_package(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS sls_logs.proto)

include_directories(${CURL_INCLUDE_DIR})

add_library(slssdk
    client.cpp
    resource.cpp
    adapter.cpp
    common.cpp
    ${PROTO_SRCS})

install(TARGETS slssdk
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(FILES client.h RestfulApiCommon.h resource.h ${PROTO_HDRS}
    DESTINATION include)
install(DIRECTORY rapidjson
    DESTINATION include)
