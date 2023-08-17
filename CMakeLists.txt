cmake_minimum_required(VERSION 3.3)

project(sls-sdk-cpp)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_SHARED_LIBS  "Enable shared library" OFF)
option(BUILD_SAMPLE "Build sample" OFF)
option(BUILD_TESTS "Build unit and perfermence tests" OFF)
option(VERBOSE "Verbose output" OFF)
option(GCC_BUILD_32 "" OFF)

set(VERBOSE ON)
set(BUILD_TESTS ON)
set(BUILD_SAMPLE ON)


# Variables
set(sls_sdk_lz4_dir "include/lz4")
set(sls_sdk_rapidjson_dir "include/rapidjson")
file(GLOB sls_sdk_srcs "*.cpp")
file(GLOB sls_sdk_headers "*.h")
set(PB_LIB "${PROJECT_NAME}-pb")
set(STATIC_LIB "${PROJECT_NAME}-static")
set(SDK_ROOT ${CMAKE_SOURCE_DIR})
set(SDK_CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(PROTOS sls_logs.proto)

# lib 
set(proto_lib_name Protobuf)
set(lz4_lib_name lz4)
set(curl_lib_name curl)

set(proto_lib protobuf::libprotobuf)
set(lz4_lib_name lz4::lz4)
set(curl_lib_name CURL::libcurl)



include(ExternalProject)
include(GNUInstallDirs)
include(cmake/platform.cmake)
include(cmake/build.cmake)
include(cmake/install.cmake)

# print debug info
if(VERBOSE)
    # platform
    message("TARGET_OS: ${TARGET_OS}")
    # option
    message("BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
    message("BUILD_SAMPLE: ${BUILD_SAMPLE}")
    message("BUILD_TESTS: ${BUILD_TESTS}")
    # proto
    message("PROTOS: ${PROTOS}")
    # target name
    message("STATIC_LIB: ${STATIC_LIB}")
    message("SHARED_LIB: ${SHARED_LIB}")
    message("PB_LIB: ${PB_LIB}")
    # srcs
    message("sls_sdk_srcs: ${sls_sdk_srcs}")
    # compiler flags
    message("SDK_COMPILER_FLAGS: ${SDK_COMPILER_FLAGS}")
endif()