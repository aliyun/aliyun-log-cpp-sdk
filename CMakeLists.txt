cmake_minimum_required(VERSION 2.6)
project(sls-sdk-cpp)

set(CMAKE_CXX_STANDARD 11)

set(ALIYUN_LOG_SDK_NAMESPACE "" CACHE STRING "Override the aliyun_log_sdk_v6 namespace")
if(NOT "${ALIYUN_LOG_SDK_NAMESPACE}" STREQUAL "")
    if(NOT "${ALIYUN_LOG_SDK_NAMESPACE}" MATCHES "^[A-Za-z_][A-Za-z0-9_]*$")
        message(FATAL_ERROR "ALIYUN_LOG_SDK_NAMESPACE must be a valid C++ identifier")
    endif()
endif()

add_library(slssdk
            client.cpp
            resource.cpp
            adapter.cpp
            common.cpp
            coding.cpp
            util.cpp
            pb.cpp)

if(NOT "${ALIYUN_LOG_SDK_NAMESPACE}" STREQUAL "")
    target_compile_definitions(slssdk PUBLIC ALIYUN_LOG_SDK_NAMESPACE=${ALIYUN_LOG_SDK_NAMESPACE})
endif()

# lib curl must be provided by user
target_include_directories(slssdk PRIVATE ${CURL_INCLUDE_DIR})
target_link_directories(slssdk PUBLIC ${CURL_LIB_DIR})


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(slssdk PUBLIC libcurl_debug)  # Linking debug version
else()
    target_link_libraries(slssdk PUBLIC curl)  # Linking release version
endif()


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

install(FILES client.h RestfulApiCommon.h resource.h pb.h sls_namespace.h
        DESTINATION include)
install(DIRECTORY rapidjson
        DESTINATION include)

option(BUILD_SAMPLE "build sample" OFF)
if(BUILD_SAMPLE)
    add_executable(sample sample.cpp)
    target_link_libraries(sample PUBLIC slssdk)
endif()

option(BUILD_TESTING "build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    add_executable(pb_test tests/pb_test.cpp)
    target_include_directories(pb_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(pb_test PRIVATE slssdk)
    add_test(NAME pb_test COMMAND pb_test)

    add_executable(sdk_test tests/sdk_test.cpp)
    target_include_directories(sdk_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(sdk_test PRIVATE slssdk)
    add_test(NAME sdk_test COMMAND sdk_test)
endif()
