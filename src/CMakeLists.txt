
find_package(Protobuf)

if(Protobuf_FOUND)
    set(PROTOBUF_LIBRARIES "protobuf::libprotobuf")
elseif((NOT PROTOBUF_LIBRARIES) OR(NOT PROTOBUF_INCLUDE_DIRS))
    message(FATAL_ERROR "Protobuf not found, PROTOBUF_LIBRARIES and PROTOBUF_INCLUDE_DIRS not set")
endif()

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS sls/sls_logs.proto)
get_filename_component(PROTO_INC_DIR "${PROTO_HDRS}" PATH)

find_package(CURL)

if(CURL_FOUND)
    set(CURL_LIBRARIES "CURL::libcurl")
elseif((NOT PROTOBUF_LIBRARIES) OR(NOT PROTOBUF_INCLUDE_DIRS))
    message(FATAL_ERROR "Protobuf not found, PROTOBUF_LIBRARIES and PROTOBUF_INCLUDE_DIRS not set")
endif()

file(GLOB SRCS sls/*.cpp)
file(GLOB LZ4_SRC_FILES sls/lz4/*.c)
add_library(${PROJECT_NAME} STATIC ${SRCS} ${LZ4_SRC_FILES} ${PROTO_SRCS})
target_link_libraries(${PROJECT_NAME}
                      PUBLIC ${PROTOBUF_LIBRARIES}
                      PRIVATE ${CURL_LIBRARIES})
target_include_directories(${PROJECT_NAME}
                            PUBLIC
                            ${PROTOBUF_INCLUDE_DIRS}
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                            $<BUILD_INTERFACE:${PROTO_INC_DIR}>
                            $<INSTALL_INTERFACE:sls>
                            PRIVATE
                            ${CURL_INCLUDE_DIRS})

# install
install(TARGETS ${PROJECT_NAME}
        EXPORT "${PROJECT_NAME}Config"
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
file(GLOB SLS_HEADERS sls/*.h)
install(FILES ${PROTO_HDRS} DESTINATION include)
install(FILES ${SLS_HEADERS} DESTINATION include/sls)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/sls/rapidjson" DESTINATION include/sls)

include(CMakePackageConfigHelpers)

# 生成 ConfigVersion.cmake 文件
write_basic_package_version_file(
  "${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMinorVersion
)

# 安装 Config.cmake 和 ConfigVersion.cmake
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION
    share/${PROJECT_NAME}
  )

install(EXPORT "${PROJECT_NAME}Config"
  FILE "${PROJECT_NAME}Config.cmake"
  DESTINATION share/${PROJECT_NAME}
  NAMESPACE ${PROJECT_NAME}::
)

