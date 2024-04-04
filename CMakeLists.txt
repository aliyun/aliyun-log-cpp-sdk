cmake_minimum_required(VERSION 3.3)

project(sls-sdk-cpp)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_SHARED_LIBS  "Enable shared library" OFF)
option(BUILD_SAMPLE "Build sample" OFF)
option(VERBOSE "Verbose output" OFF)
option(GCC_BUILD_32 "" OFF)

set(VERBOSE ON)
set(BUILD_SAMPLE ON)


# Variables
file(GLOB sls_sdk_srcs "*.cpp")
file(GLOB sls_sdk_headers "*.h")
set(SLS_LIB "${PROJECT_NAME}")
set(SDK_ROOT ${CMAKE_SOURCE_DIR})

# lib 
set(lz4_lib_name lz4)
include(ExternalProject)
include(GNUInstallDirs)

if(CMAKE_HOST_APPLE)
    set(PLATFORM_APPLE 1)
    set(TARGET_OS "APPLE")
elseif(CMAKE_HOST_UNIX)
    set(PLATFORM_LINUX 1)
    set(TARGET_OS "LINUX")
elseif(CMAKE_HOST_WIN32)
    set(PLATFORM_WINDOWS 1)
    set(TARGET_OS "WINDOWS")
else()
    message(FATAL_ERROR "Do not support unknown host OS")
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC") 
	  list(APPEND SDK_COMPILER_FLAGS "/MP")
else()
    list(APPEND SDK_COMPILER_FLAGS "-fPIC") # "-fno-exceptions"
    list(APPEND SDK_COMPILER_FLAGS "-Wall" "-pedantic" "-Wextra" "-Wno-unused-parameter") # "-Werror"
    if (GCC_BUILD_32)
        # list(APPEND SDK_COMPILER_FLAGS "-m32")
        # list(APPEND SDK_LINK_FLAGS "-m32")
    endif()
endif()

file(GLOB sls_lz4_srcs "include/lz4/*.c")
add_library(${lz4_lib_name} ${sls_lz4_srcs})
set_target_properties(${lz4_lib_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_include_directories(${lz4_lib_name} PUBLIC include/lz4)
target_compile_options(${lz4_lib_name} PRIVATE "${SDK_COMPILER_FLAGS}")
target_link_options(${lz4_lib_name} PRIVATE "${SDK_LINK_FLAGS}")  

set(lz4_FOUND TRUE)
set(lz4_INCLUDE_DIRS ${SDK_ROOT}/include/lz4)
set(lz4_LINK_LIBRARY ${lz4_lib_name})
set(lz4_LINK_LIBRARIES ${lz4_lib_name})


find_package(Protobuf REQUIRED)
find_package(CURL REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS sls_logs.proto)
get_filename_component(PROTO_BINARY_DIR "${PROTO_HDRS}" PATH)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
add_library(${SLS_LIB} ${sls_sdk_srcs} ${PROTO_SRCS})
set_target_properties(${SLS_LIB} PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(${SLS_LIB} PROPERTIES LINK_SEARCH_END_STATIC 1)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set_target_properties(${SLS_LIB}
    PROPERTIES
    LINKER_LANGUAGE CXX
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_include_directories(${SLS_LIB}
        PUBLIC ${SDK_ROOT}/include
        PUBLIC ${PROTOBUF_INCLUDE_DIRS}
        PRIVATE ${CURL_INCLUDE_DIRS}
        PUBLIC ${PROTO_BINARY_DIR})
target_link_libraries(${SLS_LIB} 
                PRIVATE CURL::libcurl
                PRIVATE ${lz4_lib_name}
                PUBLIC protobuf::libprotobuf)
target_compile_options(${SLS_LIB} 
                PRIVATE "${SDK_COMPILER_FLAGS}")
target_link_options(${SLS_LIB} 
                PRIVATE "${SDK_LINK_FLAGS}")

if(BUILD_SAMPLE)
    add_executable(sample example/sample.cpp)
    target_link_libraries(sample PRIVATE ${SLS_LIB} )
    target_compile_options(sample
                    PRIVATE ${SDK_COMPILER_FLAGS})
    target_link_options(sample
                    PRIVATE ${SDK_LINK_FLAGS})
    target_include_directories(sample PRIVATE 
        ${PROTOBUF_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR})
endif()

install(TARGETS ${SLS_LIB} ${lz4_lib_name}
        ARCHIVE DESTINATION lib)

install(FILES ${PROTO_HDRS} ${sls_sdk_headers} DESTINATION include)
install(DIRECTORY 
        ${PROTOBUF_INCLUDE_DIRS}/google
        ${CURL_INCLUDE_DIRS}/curl
        ${SDK_ROOT}/include/lz4
        ${SDK_ROOT}/include/rapidjson
        DESTINATION include)

if(VERBOSE)
    # platform
    message("TARGET_OS: ${TARGET_OS}")
    # option
    message("BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
    message("BUILD_SAMPLE: ${BUILD_SAMPLE}")
    # target name
    message("SLS_LIB: ${SLS_LIB}")
    # srcs
    message("sls_sdk_srcs: ${sls_sdk_srcs}")
    # compiler flags
    message("SDK_COMPILER_FLAGS: ${SDK_COMPILER_FLAGS}")
endif()