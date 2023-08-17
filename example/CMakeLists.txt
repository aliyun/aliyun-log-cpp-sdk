cmake_minimum_required(VERSION 2.6)

# add target
add_executable(sample ${CMAKE_CURRENT_SOURCE_DIR}/sample.cpp)

# link to sls-sdk-cpp-static lib
target_link_libraries(sample PRIVATE ${STATIC_LIB} )

target_compile_options(sample
                PRIVATE ${SDK_COMPILER_FLAGS})

target_link_options(sample
                PRIVATE ${SDK_LINK_FLAGS})

# add header files include
# target_include_directories(sample PRIVATE 
#     ${PROTOBUF_INCLUDE_DIRS}
# )