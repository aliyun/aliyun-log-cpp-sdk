cmake_minimum_required(VERSION 2.8.12)
project(sls-sdk-cpp-unittest)

include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    doctest
    PREFIX ${CMAKE_BINARY_DIR}/doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND "" #${GIT_EXECUTABLE} pull --ff-only
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")

file(GLOB UNITTEST_SRCS *_unittest.cpp)
message("unittest files: ")
foreach(unittest ${UNITTEST_SRCS})
  message(${unittest})
endforeach()

add_executable(log_unittest ${UNITTEST_SRCS})
target_link_libraries(log_unittest slssdk)
target_include_directories(log_unittest PUBLIC ${DOCTEST_INCLUDE_DIR})

include(${source_dir}/scripts/cmake/doctest.cmake)
doctest_discover_tests(log_unittest)
# add_test(NAME log_unittest COMMAND log_unittest)