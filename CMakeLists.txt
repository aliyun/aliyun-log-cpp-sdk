cmake_minimum_required(VERSION 2.8.12)
project(sls-sdk-cpp)

option(ENABLE_BUILD_SAMPLE "enable build sample" OFF)
option(ENABLE_UNITTEST "enable unittest" OFF)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# find lib protobuf & pthread & curl
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
include_directories(${CMAKE_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS sls_logs.proto)

# build lib slssdk
add_library(slssdk client.cpp
    resource.cpp
    adapter.cpp
    common.cpp
    signer.cpp
    ${PROTO_SRCS})
target_link_libraries(slssdk 
    Threads::Threads
    ${PROTOBUF_LIBRARIES}
    ${CURL_LIBRARIES}
    ${PROJECT_SOURCE_DIR}/lib/liblz4.a)


# build bin sample
if (ENABLE_BUILD_SAMPLE)
    add_executable(sample sample.cpp)
    target_link_libraries(sample slssdk)
endif()

# unittest
if (ENABLE_UNITTEST)
    enable_testing()
    add_definitions(-D__ALIYUN_LOG_UNITTEST__)
    add_subdirectory(test) 
endif()

# install lib slssdk
install(TARGETS slssdk
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
# install header files
install(FILES client.h RestfulApiCommon.h resource.h ${PROTO_HDRS}
    DESTINATION include)
install(DIRECTORY rapidjson
    DESTINATION include)

