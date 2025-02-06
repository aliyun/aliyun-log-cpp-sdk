cmake_minimum_required(VERSION 2.6)
project(sls-sdk-cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pedantic -O2 -Wno-unused-parameter -Wno-ignored-qualifiers")

add_library(slssdk
            client.cpp
            resource.cpp
            adapter.cpp
            common.cpp
            coding.cpp
            pb.cpp)

# lib curl
target_include_directories(slssdk PRIVATE ${CURL_INCLUDE_DIR})
target_link_directories(slssdk PUBLIC ${CURL_LIB_DIR})
target_link_libraries(slssdk PUBLIC curl)

# lib lz4
if ((LZ4_INCLUDE_DIR) AND (LZ4_LIB_DIR))
    target_include_directories(slssdk PRIVATE ${LZ4_INCLUDE_DIR})
    target_link_directories(slssdk PUBLIC ${LZ4_LIB_DIR})
else()
    message(STATUS "build lz4 from source")
    add_subdirectory(lz4)
    target_include_directories(slssdk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lz4)
endif()

target_link_libraries(slssdk PUBLIC lz4)

install(TARGETS slssdk
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES client.h RestfulApiCommon.h resource.h pb.h
        DESTINATION include)
install(DIRECTORY rapidjson
        DESTINATION include)

option(BUILD_SAMPLE "build sample" OFF)
if(BUILD_SAMPLE)
    add_executable(sample sample.cpp)
    target_link_libraries(sample PUBLIC slssdk)
endif()

