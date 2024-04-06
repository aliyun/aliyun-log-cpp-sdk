
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
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES} ${CURL_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROTO_INC_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})

# install
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
file(GLOB SLS_HEADERS sls/*.h)
install(FILES ${PROTO_HDRS} DESTINATION include/sls)
install(FILES ${SLS_HEADERS} DESTINATION include/sls)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/src/sls/rapidjson" DESTINATION include/sls)