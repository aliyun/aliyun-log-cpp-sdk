cmake_minimum_required(VERSION 2.6)

# add target
add_executable(slssdk-test ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
# link to sls-sdk-cpp-static lib
target_link_libraries(slssdk-test PRIVATE ${STATIC_LIB_TARGET})
# add header files include
target_include_directories(slssdk-test 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external 
            ${CMAKE_SOURCE_DIR} 
            ${CMAKE_SOURCE_DIR}/include)