# os: apple/linux/windows
# output: TARGET_OS
if(CMAKE_HOST_APPLE)
    set(PLATFORM_APPLE 1)
    set(TARGET_OS "APPLE")
elseif(CMAKE_HOST_UNIX)
    set(PLATFORM_LINUX 1)
    set(TARGET_OS "LINUX")
elseif(CMAKE_HOST_WIN32)
    set(PLATFORM_WINDOWS 1)
    set(TARGET_OS "WINDOWS")
else()
    message(FATAL_ERROR "Do not support unknown host OS")
endif()


# detect compiler: msvc / gcc
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC") 
	  list(APPEND SDK_COMPILER_FLAGS "/MP")
else()
    list(APPEND SDK_COMPILER_FLAGS "-fPIC") # "-fno-exceptions"
    list(APPEND SDK_COMPILER_FLAGS "-Wall" "-pedantic" "-Wextra") # "-Werror"
    if (GCC_BUILD_32)
        # list(APPEND SDK_COMPILER_FLAGS "-m32")
        # list(APPEND SDK_LINK_FLAGS "-m32")
    endif()
endif()


