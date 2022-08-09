cmake_minimum_required(VERSION 2.8.12)
project(sls-sdk-cpp-unittest)

file(GLOB UNITTEST_SRCS *_unittest.cpp)
message("unittest files: ")
foreach(unittest ${UNITTEST_SRCS})
  message(${unittest})
endforeach()

add_executable(log_unittest ${UNITTEST_SRCS})
target_link_libraries(log_unittest slssdk)
target_include_directories(log_unittest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# include(${CMAKE_CURRENT_SOURCE_DIR}/doctest/cmake/doctest.cmake)
# doctest_discover_tests(log_unittest)
add_test(NAME log_unittest COMMAND log_unittest)
