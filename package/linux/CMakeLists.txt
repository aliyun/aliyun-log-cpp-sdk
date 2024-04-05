cmake_minimum_required(VERSION 3.3)

project(slssdk-lib-example)
set(CMAKE_CXX_STANDARD 11)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIB_DIR "${PROJECT_SOURCE_DIR}/Debug")
else()
    set(LIB_DIR "${PROJECT_SOURCE_DIR}/Release")
endif()

add_executable(example example.cpp)
target_include_directories(example PUBLIC include)

target_link_libraries(example PUBLIC 
                    "${LIB_DIR}/libslssdk.a"
                    "${LIB_DIR}/libcurl.a"
                    "${LIB_DIR}/libssl.a"
                    "${LIB_DIR}/libcrypto.a"
                    "${LIB_DIR}/libz.a"
                    "${LIB_DIR}/libprotobuf.a"
                    -lpthread ${CMAKE_DL_LIBS})
