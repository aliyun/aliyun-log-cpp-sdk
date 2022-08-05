cmake_minimum_required(VERSION 2.6)
project(sls-sdk-cpp)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS sls_logs.proto)

set(LibSrcFiles 
    client.cpp
    resource.cpp
    adapter.cpp
    common.cpp
    signer.cpp
    ${PROTO_SRCS})

add_library(slssdk ${LibSrcFiles})
target_link_libraries(slssdk Threads::Threads ${PROTOBUF_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_SOURCE_DIR}/lib/liblz4.a)

add_executable(sample sample.cpp)
target_link_libraries(sample slssdk)

add_executable(unittest test.cpp)
target_link_libraries(unittest slssdk)

enable_testing()
add_test(NAME unittest COMMAND unittest)

install(TARGETS slssdk
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(FILES client.h RestfulApiCommon.h resource.h ${PROTO_HDRS}
    DESTINATION include)
install(DIRECTORY rapidjson
    DESTINATION include)

