cmake_minimum_required(VERSION 3.3)

project(slssdk-lib-example)
set(CMAKE_CXX_STANDARD 11)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIB_DIR "${PROJECT_SOURCE_DIR}/Debug")
else()
    set(LIB_DIR "${PROJECT_SOURCE_DIR}/Release")
endif()

add_executable(example example.cpp)
target_include_directories(example PUBLIC include include/sls)

target_link_libraries(example PUBLIC 
                    "${LIB_DIR}/slssdk.lib"
                    "${LIB_DIR}/libcurl.lib"
                    "${LIB_DIR}/zlib.lib"
                    "${LIB_DIR}/libprotobuf.lib"
                    ${CMAKE_DL_LIBS}
                    "Crypt32.lib")
