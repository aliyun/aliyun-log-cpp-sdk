cmake_minimum_required(VERSION 2.6)
project(sls-sdk-cpp)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS sls_logs.proto)
add_library(slssdk STATIC
    client.cpp
    resource.cpp
    adapter.cpp
    common.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS})
include_directories(slssdk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(slssdk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(slssdk PUBLIC ${PROTOBUF_INCLUDE_DIRS})

find_package(CURL REQUIRED)
target_include_directories(slssdk PUBLIC ${CURL_INCLUDE_DIR})

find_package(lz4 REQUIRED)
target_include_directories(slssdk PUBLIC ${LZ4_INCLUDE_DIR})

target_link_libraries(slssdk protobuf::libprotobuf CURL::libcurl lz4::lz4)
if(WIN32)
    target_link_libraries(slssdk ws2_32)
endif()

add_executable(sample sample.cpp)
target_link_libraries(sample slssdk protobuf::libprotobuf CURL::libcurl lz4::lz4)

install(TARGETS slssdk
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(FILES client.h RestfulApiCommon.h resource.h ${PROTO_HDRS}
    DESTINATION include)

install(DIRECTORY include/rapidjson
    DESTINATION include)
