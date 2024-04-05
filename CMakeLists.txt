cmake_minimum_required(VERSION 3.3)

option(BUILD_EXAMPLE "Build sample" OFF)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message("VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")

project(slssdk)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(VERBOSE ON)
set(BUILD_EXAMPLE ON)

if(CMAKE_HOST_UNIX)
    set(PLATFORM linux)
elseif(CMAKE_HOST_WIN32)
    set(PLATFORM windows)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_definitions(_LARGEFILE64_SOURCE)
    set(COMMON_FLAG "-Wall -fpic -Werror -Wno-unused-parameter -Wno-pedantic -Wno-deprecated-declarations")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAG}")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wno-class-memaccess ${COMMON_FLAG}")
    message("gcc c compile flags: ${CMAKE_C_FLAGS}")
    message("gcc cxx compile flags: ${CMAKE_CXX_FLAGS}")
elseif(MSVC)
    set(COMMON_FLAG "/W4 /WX /fp:precise /wd4996 /wd4100 /wd4244 /wd4267 /wd4245")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAG}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAG}")
    add_compile_definitions(PROTOBUF_USE_DLLS)
    message("MSVC c compile flags: ${CMAKE_C_FLAGS}")
    message("MSVC cxx compile flags: ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

add_subdirectory(src)

if(VERBOSE)
    # platform
    message("TARGET_OS: ${TARGET_OS}")

    # option
    message("BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
    message("BUILD_EXAMPLE: ${BUILD_EXAMPLE}")
    message("PLATFORM: ${PLATFORM}")
endif()

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()