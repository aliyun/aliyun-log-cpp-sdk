cmake_minimum_required(VERSION 3.3)
project(sls-sdk-cpp)

# Options
option(BUILD_SHARED_LIBS  "Enable shared library" OFF)
option(BUILD_SAMPLE "Build sample" OFF)
option(BUILD_TESTS "Build unit and perfermence tests" OFF)
option(VERBOSE "Verbose output" OFF)
option(PUBLISH "Publish to release" OFF)

# set(VERBOSE ON)
# set(BUILD_SHARED_LIBS ON)
# set(BUILD_TESTS ON)
# set(BUILD_SAMPLE ON)
# set(PUBLISH ON)

# Variables
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(sls_sdk_lz4_dir "include/lz4")
set(sls_sdk_rapidjson_dir "include/rapidjson")
file(GLOB sls_sdk_srcs "*.cpp")
file(GLOB sls_sdk_headers "*.h")

# Determine name of targets
set(PB_LIB "${PROJECT_NAME}-pb")
set(STATIC_LIB "${PROJECT_NAME}-static")
set(SHARED_LIB "${PROJECT_NAME}-shared")

# find package ptotobuf / curl 
find_package(Protobuf REQUIRED)
find_package(CURL REQUIRED)
include(Findlz4.cmake)
if (NOT lz4_FOUND)
    message(FATAL_ERROR "library lz4 not found}")
endif()

# protobuf generate
set(PROTOS sls_logs.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOS})
get_filename_component(PROTO_BINARY_DIR "${PROTO_HDRS}" PATH)

# libs and includes of third party libraries
# private: lz4 curl
# public: protobuf
set(LIBS_INSTALL_TARGETS protobuf::libprotobuf CURL::libcurl)

# print debug info
if(VERBOSE)
    # option
    message("BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
    message("BUILD_SAMPLE: ${BUILD_SAMPLE}")
    message("BUILD_TESTS: ${BUILD_TESTS}")
    message("PUBLISH: ${PUBLISH}")
    # proto
    message("PROTOS: ${PROTOS}")
    message("PROTO_BINARY_DIR: ${PROTO_BINARY_DIR}")
    # target name
    message("STATIC_LIB: ${STATIC_LIB}")
    message("SHARED_LIB: ${SHARED_LIB}")
    message("PB_LIB: ${PB_LIB}")
    # srcs
    message("sls_sdk_srcs: ${sls_sdk_srcs}")
endif()

# static lib
add_library(${STATIC_LIB} STATIC
    ${sls_sdk_srcs}
    ${PROTO_SRCS})

set_target_properties(${STATIC_LIB}
    PROPERTIES
    LINKER_LANGUAGE CXX
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

target_include_directories(${STATIC_LIB}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${sls_sdk_lz4_dir}
        PUBLIC ${PROTOBUF_INCLUDE_DIRS}
        PRIVATE ${CURL_INCLUDE_DIRS}
        PUBLIC ${PROTO_BINARY_DIR})
        
target_link_libraries(${STATIC_LIB} 
                PUBLIC -static
                PRIVATE CURL::libcurl
                PRIVATE lz4::lz4
                PRIVATE protobuf::libprotobuf)

# shared lib
if (BUILD_SHARED_LIBS)
    add_library(${SHARED_LIB} SHARED
                ${sls_sdk_srcs}
                ${PROTO_SRCS})
    
    set_target_properties(${SHARED_LIB}
        PROPERTIES
        LINKER_LANGUAGE CXX
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    
    target_include_directories(${SHARED_LIB}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${CURL_INCLUDE_DIRS}
        PUBLIC ${PROTOBUF_INCLUDE_DIRS}
        PUBLIC ${PROTO_BINARY_DIR})
    
    target_link_libraries(${SHARED_LIB} 
        PRIVATE CURL::libcurl
        PUBLIC lz4::lz4
        PUBLIC protobuf::libprotobuf)
endif()

# install static lib
install(TARGETS ${STATIC_LIB}
        ARCHIVE DESTINATION lib)

# install headers
install(FILES 
        ${sls_sdk_headers} 
        ${PROTO_HDRS}
        DESTINATION include)
install(DIRECTORY 
        ${sls_sdk_rapidjson_dir} 
        ${sls_sdk_lz4_dir}
        DESTINATION include)

if (PUBLISH)
    # more headers when publish
    install(DIRECTORY 
        ${CURL_INCLUDE_DIRS}/curl
        ${PROTOBUF_INCLUDE_DIRS}/google
        DESTINATION include)
    # libs when win32
    if (WIN32)
        install(FILES
            $<TARGET_LINKER_FILE:CURL::libcurl>
            $<TARGET_LINKER_FILE:protobuf::libprotobuf>
            $<TARGET_LINKER_FILE:lz4::lz4>
            DESTINATION lib)

        install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/example/sample.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/example/windows-standalone/CMakeLists.txt
            DESTINATION .
        )

    endif()
endif()

# install shared lib
if (BUILD_SHARED_LIBS)
    install(TARGETS ${SHARED_LIB}
            RUNTIME DESTINATION lib
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)

    if (WIN32 AND PUBLISH)
        # shared dlls
        install(FILES
                $<TARGET_RUNTIME_DLLS:${SHARED_LIB}>
                DESTINATION lib)
    endif()
endif()

# sample
if(BUILD_SAMPLE)
	add_subdirectory(example)
endif()

# test
if(BUILD_TESTS)
	add_subdirectory(test)
endif()