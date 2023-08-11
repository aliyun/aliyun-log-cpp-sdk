cmake_minimum_required(VERSION 2.6)
project(sls-sdk-cpp)

#Options
option(BUILD_SHARED_LIBS  "Enable shared library" OFF)
option(BUILD_SAMPLE "Build sample" OFF)
option(BUILD_TESTS "Build unit and perfermence tests" OFF)
option(VERBOSE "Verbose output" OFF)

set(VERBOSE ON)
# set(BUILD_SHARED_LIBS ON)
# set(BUILD_TESTS ON)
# set(BUILD_SAMPLE ON)

# variables
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(PROTO_FILES sls_logs.proto)
file(GLOB sls_sdk_srcs "*.cpp")
file(GLOB sls_sdk_headers "*.h")
set(sls_sdk_lz4_dir "include/lz4")
set(sls_sdk_rapidjson_dir "include/rapidjson")

# determine target name
set(STATIC_LIB_TARGET "${PROJECT_NAME}-static")
set(SHARED_LIB_TARGET "${PROJECT_NAME}-shared")

# find package ptotobuf / curl / lz4
find_package(Protobuf REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(lz4 CONFIG REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})
get_filename_component(PROTO_INCLUDE_DIR "${PROTO_HDRS}" PATH)

# libs and includes of third party libraries
set(LIB_INCLUDE_DIRS_PRIVATE ${CURL_INCLUDE_DIRS})
set(LIB_LINK_LIBRARIES_PRIVATE CURL::libcurl)
set(LIB_INCLUDE_DIRS_PUBLIC ${PROTOBUF_INCLUDE_DIRS} ${LZ4_INCLUDE_DIRS})
set(LIB_LINK_LIBRARIES_PUBLIC protobuf::libprotobuf lz4::lz4)

if (WIN32)
    set(LIB_LINK_LIBRARIES_PRIVATE ${LIB_LINK_LIBRARIES_PRIVATE} ws2_32)
endif()

# print debug info
if(VERBOSE)
    # option
    message("BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
    message("BUILD_SAMPLE: ${BUILD_SAMPLE}")
    message("BUILD_TESTS: ${BUILD_TESTS}")
    # proto
    message("PROTO_FILES: ${PROTO_FILES}")
    message("PROTO_SRCS: ${PROTO_SRCS}")
    message("PROTO_HDRS: ${PROTO_HDRS}")
    message("PROTO_INCLUDE_DIR: ${PROTO_INCLUDE_DIR}")
    # target name
    message("STATIC_LIB_SUFFIX: ${STATIC_LIB_SUFFIX}")
    message("STATIC_LIB_TARGET: ${STATIC_LIB_TARGET}")
    message("SHARED_LIB_TARGET: ${SHARED_LIB_TARGET}")
    # includes and libs
    message("LIB_INCLUDE_DIRS_PRIVATE: ${LIB_INCLUDE_DIRS_PRIVATE}")
    message("LIB_LINK_LIBRARIES_PRIVATE: ${LIB_LINK_LIBRARIES_PRIVATE}")
    message("LIB_INCLUDE_DIRS_PUBLIC: ${LIB_INCLUDE_DIRS_PUBLIC}")
    message("LIB_LINK_LIBRARIES_PUBLIC: ${LIB_LINK_LIBRARIES_PUBLIC}")
    # srcs
    message("sls_sdk_srcs: ${sls_sdk_srcs}")
endif()

# static lib
add_library(${STATIC_LIB_TARGET} STATIC
    ${sls_sdk_srcs}
    ${PROTO_SRCS})
    
set_target_properties(${STATIC_LIB_TARGET}
    PROPERTIES
    LINKER_LANGUAGE CXX
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}${STATIC_LIB_TARGET})

target_include_directories(${STATIC_LIB_TARGET}
        PUBLIC include
        PUBLIC ${PROTO_INCLUDE_DIR}
        PRIVATE ${LIB_INCLUDE_DIRS_PRIVATE}
        PUBLIC ${LIB_INCLUDE_DIRS_PUBLIC})
target_link_libraries(${STATIC_LIB_TARGET} 
                PRIVATE ${LIB_LINK_LIBRARIES_PRIVATE}
                PUBLIC ${LIB_LINK_LIBRARIES_PUBLIC})

# shared lib
if (BUILD_SHARED_LIBS)
    add_library(${SHARED_LIB_TARGET} SHARED
                ${sls_sdk_srcs}
                ${PROTO_SRCS})
    
    set_target_properties(${SHARED_LIB_TARGET}
        PROPERTIES
        LINKER_LANGUAGE CXX
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}${SHARED_LIB_TARGET})
    
    target_include_directories(${SHARED_LIB_TARGET}
        PUBLIC include
        PUBLIC ${PROTO_INCLUDE_DIR}
        PRIVATE ${LIB_INCLUDE_DIRS_PRIVATE}
        PUBLIC ${LIB_INCLUDE_DIRS_PUBLIC})
    
    target_link_libraries(${SHARED_LIB_TARGET} 
        PRIVATE ${LIB_LINK_LIBRARIES_PRIVATE}
        PUBLIC ${LIB_LINK_LIBRARIES_PUBLIC})
endif()

# install
install(TARGETS ${STATIC_LIB_TARGET}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
install(FILES ${sls_sdk_headers} ${PROTO_HDRS}
        DESTINATION include)
install(DIRECTORY ${sls_sdk_rapidjson_dir}
        DESTINATION include)
install(DIRECTORY ${sls_sdk_lz4_dir}
        DESTINATION include)

if (BUILD_SHARED_LIBS)
install(TARGETS ${SHARED_LIB_TARGET}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()

# sample
if(BUILD_SAMPLE)
	add_subdirectory(example)
endif()

# test
if(BUILD_TESTS)
	add_subdirectory(test)
endif()