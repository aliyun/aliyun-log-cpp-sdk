

add_library(${lz4_lib_name} ${sls_sdk_lz4_srcs})

set_target_properties(${lz4_lib_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

target_include_directories(${lz4_lib_name}
    PUBLIC ${sls_sdk_lz4_dir})

target_compile_options(${lz4_lib_name}
    PRIVATE "${SDK_COMPILER_FLAGS}")

target_link_options(${lz4_lib_name} 
    PRIVATE "${SDK_LINK_FLAGS}")  

set(lz4_FOUND TRUE)
set(lz4_INCLUDE_DIRS ${SDK_ROOT}/include/lz4)
set(lz4_LINK_LIBRARY ${lz4_lib_name})
set(lz4_LINK_LIBRARIES ${lz4_lib_name})

