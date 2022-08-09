cmake_minimum_required(VERSION 2.8.12)
project(sls-sdk-cpp)

option(ENABLE_BUILD_SAMPLE "enable build sample" OFF)
option(ENABLE_UNITTEST "enable unittest" OFF)

# using c++ 11
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# set output dir
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

# generate protobuf files    
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS sls_logs.proto)



# target lib [slssdk]
add_library(slssdk client.cpp
    resource.cpp
    adapter.cpp
    common.cpp
    signer.cpp
    ${PROTO_SRCS})

# link protobuf & curl & lz4   
target_link_libraries(slssdk 
    ${PROTOBUF_LIBRARIES}
    ${CURL_LIBRARIES}
    ${PROJECT_SOURCE_DIR}/lib/liblz4.a)

# link pthread, compatiable with cmake 2.8.12+
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(slssdk PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(slssdk "${CMAKE_THREAD_LIBS_INIT}")
endif()


# build bin [sample]
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

