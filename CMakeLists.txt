cmake_minimum_required(VERSION 2.6)
project(sls-sdk-cpp)

set(CMAKE_CXX_STANDARD 11)

add_library(slssdk
            client.cpp
            resource.cpp
            adapter.cpp
            common.cpp
            coding.cpp
            util.cpp
            pb.cpp)

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

install(FILES client.h RestfulApiCommon.h resource.h pb.h
        DESTINATION include)
install(DIRECTORY rapidjson
        DESTINATION include)

option(BUILD_SAMPLE "build sample" OFF)
if(BUILD_SAMPLE)
    add_executable(sample sample.cpp)
    target_link_libraries(sample PUBLIC slssdk)
endif()

