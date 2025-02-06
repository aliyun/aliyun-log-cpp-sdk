cmake_minimum_required(VERSION 2.6)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -pedantic -O3 -Wundef -Wshadow -Wcast-align -Wstrict-prototypes")

add_library(lz4 lz4.c xxhash.c)

install(TARGETS lz4
        DESTINATION lib
        ARCHIVE DESTINATION lib)
