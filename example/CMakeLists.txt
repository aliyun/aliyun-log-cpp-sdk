add_executable(example ${PROJECT_SOURCE_DIR}/example/example.cpp)
target_link_libraries(example ${PROJECT_NAME})
target_include_directories(example PRIVATE ${PROJECT_SOURCE_DIR}/src)