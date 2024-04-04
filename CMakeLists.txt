cmake_minimum_required(VERSION 3.3)

project(slssdk)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_SHARED_LIBS  "Enable shared library" OFF)
option(BUILD_EXAMPLE "Build sample" OFF)
set(VERBOSE ON)
set(BUILD_EXAMPLE ON)

# if(CMAKE_HOST_APPLE)
#     set(PLATFORM_APPLE 1)
#     set(TARGET_OS "APPLE")
if(CMAKE_HOST_UNIX)
    set(PLATFORM linux)
elseif(CMAKE_HOST_WIN32)
    set(PLATFORM windows)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(COMMON_FLAG "-Wall -Werror -fpic -fPIC -Wno-unused-parameter -D_LARGEFILE64_SOURCE")
set(CMAKE_C_FLAGS " -g -ggdb -O0 ${COMMON_FLAG}")
set(CMAKE_CXX_FLAGS " -g -ggdb -O0 ${COMMON_FLAG}")

set(CMAKE_C_FLAGS_DEBUG " -g -ggdb -O0 ${COMMON_FLAG}")
set(CMAKE_CXX_FLAGS_DEBUG " -g -ggdb -O0 ${COMMON_FLAG}")

set(CMAKE_C_FLAGS_RELEASE " -O3 ${COMMON_FLAG}")
set(CMAKE_CXX_FLAGS_RELEASE " -O3 ${COMMON_FLAG}")

set(CMAKE_C_FLAGS_MINSIZEREF " -Os ${COMMON_FLAG}")
set(CMAKE_CXX_FLAGS_MINSIZEREF " -Os ${COMMON_FLAG}")

set(CMAKE_C_FLAGS_RELWITHDEBINFO " -O2 ${COMMON_FLAG}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO " -O2 ${COMMON_FLAG}")

set(CMAKE_C_FLAGS_COVERAGE " ${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_COVERAGE " ${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverag")

set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

add_subdirectory(src)

if(VERBOSE)
    # platform
    message("TARGET_OS: ${TARGET_OS}")
    # option
    message("BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
    message("BUILD_EXAMPLE: ${BUILD_EXAMPLE}")
    # compiler flags
    message("SDK_COMPILER_FLAGS: ${SDK_COMPILER_FLAGS}")
    message("PLATFORM: ${PLATFORM}")
endif()

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()