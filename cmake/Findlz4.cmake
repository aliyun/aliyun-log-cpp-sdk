
find_package(lz4 CONFIG)

if (NOT lz4_FOUND)
    if (UNIX)
        if (VERBOSE)
          message("use bundled lz4 lib")
        endif()

        add_library(lz4::lz4 STATIC IMPORTED)
        set_target_properties(lz4::lz4 PROPERTIES
            IMPORTED_LOCATION ${SDK_ROOT}/lib/liblz4.so.1.9.4
            IMPORTED_IMPLIB ${SDK_ROOT}/lib/liblz4.a 
            INTERFACE_INCLUDE_DIRECTORIES ${SDK_ROOT}/include)
        set(lz4_FOUND TRUE)
        set(lz4_INCLUDE_DIRS ${SDK_ROOT}/include)
        set(lz4_LINK_LIBRARY lz4::lz4)
        set(lz4_LINK_LIBRARIES lz4::lz4 )
    endif()
endif()


