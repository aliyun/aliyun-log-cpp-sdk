OUT_DIR=out
target=$1
TRIPLET=${TRIPLET:-x64-linux}

VCPKG_TRIPLET_DIR=${VCPKG_ROOT}/installed/${TRIPLET}
echo "VCPKG_ROOT=${VCPKG_ROOT}"
echo "TRIPLET=${TRIPLET}"

if [ -d "build" ]; then
    rm -rf build
fi
if [ -d "${OUT_DIR}" ]; then
    rm -rf ${OUT_DIR}
fi
mkdir build && mkdir -p ${OUT_DIR}

# ==== 构建 Debug 版本 ==== 
if [ "$target" = "All" -o "$target" = "Debug" ]; then
    echo "build on debug Release"
    mkdir -p  ${OUT_DIR}/Debug
    cmake --preset Debug -DVCPKG_TARGET_TRIPLET="${TRIPLET}"
    cmake --build --preset Debug 
    cmake --install build --prefix "${OUT_DIR}" --config Debug
    # 拷贝 lib 到对应目录
    cp ${OUT_DIR}/lib/*.a "${OUT_DIR}/Debug/"
    cp ${VCPKG_TRIPLET_DIR}/debug/lib/libcrypto.a ${OUT_DIR}/Debug/libcrypto.a
    cp ${VCPKG_TRIPLET_DIR}/debug/lib/libssl.a ${OUT_DIR}/Debug/libssl.a
    cp ${VCPKG_TRIPLET_DIR}/debug/lib/libcurl-d.a ${OUT_DIR}/Debug/libcurl.a
    cp ${VCPKG_TRIPLET_DIR}/debug/lib/libprotobufd.a ${OUT_DIR}/Debug/libprotobuf.a
    cp ${VCPKG_TRIPLET_DIR}/debug/lib/libz.a ${OUT_DIR}/Debug/libz.a
fi


# ==== 构建 Release 版本 ====
if [ "$target" = "All" -o "$target" = "Release" ]; then
  echo "build on linux Release"
  rm -rf build && mkdir build && mkdir -p ${OUT_DIR}/Release
  cmake --preset Release -DVCPKG_TARGET_TRIPLET="${TRIPLET}"
  cmake --build --preset Release 
  cmake --install build --prefix "${OUT_DIR}" --config Release
  # 拷贝 lib 到对应目录
  RELEASE_LIBS=(libcrypto.a libssl.a libcurl.a libprotobuf.a libz.a)
  for lib in ${RELEASE_LIBS[@]}; do
      cp ${VCPKG_TRIPLET_DIR}/lib/${lib} ${OUT_DIR}/Release/${lib}
  done
  cp ${OUT_DIR}/lib/*.a ${OUT_DIR}/Release/
fi

# 拷贝头文件、exmaple 文件
mkdir -p out/include
cp -rf "${VCPKG_TRIPLET_DIR}/include/curl" "${OUT_DIR}/include/curl"
cp -rf "${VCPKG_TRIPLET_DIR}/include/google" "${OUT_DIR}/include/google"
cp -rf "${VCPKG_TRIPLET_DIR}/include/openssl" "${OUT_DIR}/include/openssl"
cp package/linux/* "${OUT_DIR}/"
cp "example/example.cpp" "${OUT_DIR}/"
rm -rf ${OUT_DIR}/lib

tree ${OUT_DIR} -L 3