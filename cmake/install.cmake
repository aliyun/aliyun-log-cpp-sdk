
# ==== install ====
# install static lib
install(TARGETS ${STATIC_LIB}
        ARCHIVE DESTINATION lib)
        
install(FILES ${PROTO_HDRS} DESTINATION include)
install(DIRECTORY 
        ${PROTOBUF_INCLUDE_DIRS}/google
        ${CURL_INCLUDE_DIRS}/curl
        DESTINATION include)

                
if (BUILD_SAMPLE)
  install(TARGETS sample 
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    RUNTIME DESTINATION bin)
endif()

if (BUILD_TESTS)
  install(TARGETS slssdk-test 
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ    
    RUNTIME DESTINATION bin)
endif()



