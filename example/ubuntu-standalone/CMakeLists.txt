# cmake -B build -DCMAKE_BUILD_TYPE=Release
# cmake --build build --clean-first --config Release

cmake_minimum_required(VERSION 3.5)
project(sample)

set(CMAKE_BUILD_TYPE Release)

# add target
add_executable(sample sample.cpp)
# link to sls-sdk-cpp-static lib
target_link_directories(sample PRIVATE lib)
target_link_libraries(sample PRIVATE sls-sdk-cpp-static curl lz4 protobuf pthread)


# add header files include
target_include_directories(sample PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/lz4)