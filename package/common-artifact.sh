DST_INCLUDE_DIR=${OUT_DIR}/include
mkdir -p ${DST_INCLUDE_DIR}
cp -rf include/rapidjson ${DST_INCLUDE_DIR}/rapidjson
cp src/*.h ${DST_INCLUDE_DIR}/

cp -rf ${VCPKG_TRIPLET_DIR}/include/curl ${DST_INCLUDE_DIR}/curl
cp -rf ${VCPKG_TRIPLET_DIR}/include/openssl ${DST_INCLUDE_DIR}/openssl
cp -rf ${VCPKG_TRIPLET_DIR}/include/google ${DST_INCLUDE_DIR}/google
