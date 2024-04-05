OUT_DIR=$1
TRIPLET=$($TRIPLET:-x64-linux)

VCPKG_TRIPLET_DIR=${VCPKG_ROOT}/installed/${TRIPLET}
echo "VCPKG_ROOT=${VCPKG_ROOT}"
echo "TRIPLET=${TRIPLET}"

if [ -d build ]; then
    rm -rf build
fi
if [-d "${OUT_DIR}" ]; then
    rm -rf ${OUT_DIR}
fi
mkdir build && mkdir -p ${OUT_DIR}


# ==== 构建 Debug 版本 ==== 
echo "build on debug Release"
mkdir -p  ${OUT_DIR}/Debug
cmake --preset Debug -DVCPKG_TARGET_TRIPLET="${TRIPLET}"
cmake --build --preset Debug 
cmake --install build --prefix "${OUT_DIR}" --config Debug
# 拷贝 lib 到对应目录
Copy-Item "${OUT_DIR}/lib/*.a" "${OUT_DIR}/Debug/"
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libcrypto.a ${OUT_DIR}/Debug/libcrypto.a
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libssl.a ${OUT_DIR}/Debug/libssl.a
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libcurl-d.a ${OUT_DIR}/Debug/libcurl.a
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libprotobufd.a ${OUT_DIR}/Debug/libprotobuf.a
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libz.a ${OUT_DIR}/Debug/libz.a


# # ==== 构建 Release 版本 ====
# echo "build on linux Release"
# rm -rf build && mkdir build
# cmake --preset Release -DVCPKG_TARGET_TRIPLET="${TRIPLET}"
# cmake --build --preset Release 
# cmake --install build --prefix "${OUT_DIR}" --config Release

# DST_INCLUDE_DIR=${OUT_DIR}/include
# mkdir -p ${DST_INCLUDE_DIR}
# cp -rf include/rapidjson ${DST_INCLUDE_DIR}/rapidjson
# cp src/*.h ${DST_INCLUDE_DIR}/

# cp -rf ${VCPKG_TRIPLET_DIR}/include/curl ${DST_INCLUDE_DIR}/curl
# cp -rf ${VCPKG_TRIPLET_DIR}/include/openssl ${DST_INCLUDE_DIR}/openssl
# cp -rf ${VCPKG_TRIPLET_DIR}/include/google ${DST_INCLUDE_DIR}/google


# CUR_DIR=$(dirname "$0")
# bash ${CUR_DIR}/common-artifact.sh

# mkdir -p ${OUT_DIR}/linux/Release
# mkdir -p ${OUT_DIR}/linux/Debug

# cp ${VCPKG_TRIPLET_DIR}/lib/libcrypto.a ${OUT_DIR}/linux/Release/libcrypto.a
# cp ${VCPKG_TRIPLET_DIR}/lib/libssl.a ${OUT_DIR}/linux/Release/libssl.a
# cp ${VCPKG_TRIPLET_DIR}/lib/libcurl.a ${OUT_DIR}/linux/Release/libcurl.a
# cp ${VCPKG_TRIPLET_DIR}/lib/libprotobuf.a ${OUT_DIR}/linux/Release/libprotobuf.a
# cp ${VCPKG_TRIPLET_DIR}/lib/libz.a ${OUT_DIR}/linux/Release/libz.a

# cp ${VCPKG_TRIPLET_DIR}/debug/lib/libcrypto.a ${OUT_DIR}/linux/Debug/libcrypto.a
# cp ${VCPKG_TRIPLET_DIR}/debug/lib/libssl.a ${OUT_DIR}/linux/Debug/libssl.a
# cp ${VCPKG_TRIPLET_DIR}/debug/lib/libcurl-d.a ${OUT_DIR}/linux/Debug/libcurl.a
# cp ${VCPKG_TRIPLET_DIR}/debug/lib/libprotobufd.a ${OUT_DIR}/linux/Debug/libprotobuf.a
# cp ${VCPKG_TRIPLET_DIR}/debug/lib/libz.a ${OUT_DIR}/linux/Debug/libz.a


# cmake --preset=release && cmake --build build --clean-first \
# && cmake --install build --prefix ${OUT_DIR} && \
# mv ${OUT_DIR}/lib/libslssdk.a ${OUT_DIR}/linux/Release/

# cmake --preset=debug && cmake --build build --clean-first \
# && cmake --install build --prefix ${OUT_DIR} && \
# mv ${OUT_DIR}/lib/libslssdk.a ${OUT_DIR}/linux/Debug/

# cp package/CMakeLists-linux.txt ${OUT_DIR}/linux/CMakeLists.txt
# rmdir ${OUT_DIR}/lib
# cp example/example.cpp ${OUT_DIR}/
# tree ${OUT_DIR}
