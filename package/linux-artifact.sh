export OUT_DIR="$(pwd)/out"
export VCPKG_TRIPLET_DIR=${VCPKG_ROOT}/installed/x64-linux
CUR_DIR=$(dirname "$0")
bash ${CUR_DIR}/common-artifact.sh

mkdir -p ${OUT_DIR}/linux/Release
mkdir -p ${OUT_DIR}/linux/Debug

cp ${VCPKG_TRIPLET_DIR}/lib/libcrypto.a ${OUT_DIR}/linux/Release/libcrypto.a
cp ${VCPKG_TRIPLET_DIR}/lib/libssl.a ${OUT_DIR}/linux/Release/libssl.a
cp ${VCPKG_TRIPLET_DIR}/lib/libcurl.a ${OUT_DIR}/linux/Release/libcurl.a
cp ${VCPKG_TRIPLET_DIR}/lib/libprotobuf.a ${OUT_DIR}/linux/Release/libprotobuf.a
cp ${VCPKG_TRIPLET_DIR}/lib/libz.a ${OUT_DIR}/linux/Release/libz.a

cp ${VCPKG_TRIPLET_DIR}/debug/lib/libcrypto.a ${OUT_DIR}/linux/Debug/libcrypto.a
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libssl.a ${OUT_DIR}/linux/Debug/libssl.a
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libcurl-d.a ${OUT_DIR}/linux/Debug/libcurl.a
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libprotobufd.a ${OUT_DIR}/linux/Debug/libprotobuf.a
cp ${VCPKG_TRIPLET_DIR}/debug/lib/libz.a ${OUT_DIR}/linux/Debug/libz.a


cmake --preset=release && cmake --build build --clean-first \
&& cmake --install build --prefix ${OUT_DIR} && \
mv ${OUT_DIR}/lib/libslssdk.a ${OUT_DIR}/linux/Release/

cmake --preset=debug && cmake --build build --clean-first \
&& cmake --install build --prefix ${OUT_DIR} && \
mv ${OUT_DIR}/lib/libslssdk.a ${OUT_DIR}/linux/Debug/

cp package/CMakeLists-linux.txt ${OUT_DIR}/linux/CMakeLists.txt
rmdir ${OUT_DIR}/lib
cp example/example.cpp ${OUT_DIR}/
tree ${OUT_DIR}
