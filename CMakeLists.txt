cmake_minimum_required(VERSION 2.6)
project(sls-sdk-cpp)

include_directories(${CURL_INCLUDE_DIR})

add_library(slssdk
    client.cpp
    resource.cpp
    adapter.cpp
    common.cpp
    coding.cpp
    pb.cpp)

install(TARGETS slssdk
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(FILES client.h RestfulApiCommon.h resource.h pb.h
    DESTINATION include)
install(DIRECTORY rapidjson
    DESTINATION include)
