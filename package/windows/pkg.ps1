
$OUT_DIR=$args[0]
$VCPKG_ROOT=$env:VCPKG_ROOT

# 清空目录
if (Test-Path build) {
  Remove-Item -Recurse -Force build
}
if (Test-Path ${OUT_DIR}) {
  Remove-Item -Recurse -Force ${OUT_DIR}
}
New-Item -ItemType Directory -Force -Path ${OUT_DIR}

# 默认值 x64-windows-static-md
$TRIPLET = If ($null -ne ${VCPKG_TARGET_TRIPLET}) {${VCPKG_TARGET_TRIPLET}} Else {"x64-windows-static-md"}
$VCPKG_TRIPLET_DIR = "${VCPKG_ROOT}/installed/${TRIPLET}"
Write-Host "VCPKG_ROOT: ${VCPKG_ROOT}"
Write-Host "TRIPLET: ${TRIPLET}"

# ==== 构建 Debug 版本 ==== 
Write-Host "build on windows Debug"
New-Item -ItemType Directory -Force -Path "${OUT_DIR}/Debug"
cmake --preset Debug -DVCPKG_TARGET_TRIPLET="${TRIPLET}"
cmake --build --preset Debug 
cmake --install build --prefix "${OUT_DIR}" --config Debug
# 拷贝 lib 到对应目录
Copy-Item "${OUT_DIR}/lib/*.lib" "${OUT_DIR}/Debug/"
Copy-Item "${VCPKG_TRIPLET_DIR}/debug/lib/zlibd.lib" "${OUT_DIR}/Debug/zlib.lib"
Copy-Item "${VCPKG_TRIPLET_DIR}/debug/lib/libcurl-d.lib" "${OUT_DIR}/Debug/libcurl.lib"
Copy-Item "${VCPKG_TRIPLET_DIR}/debug/lib/libprotobufd.lib" "${OUT_DIR}/Debug/libprotobuf.lib"

# ==== 构建 Release 版本 ====
Write-Host "build on windows Release"
Remove-Item -Recurse -Force build # clean build directory
New-Item -ItemType Directory -Force -Path "${OUT_DIR}/Release"
cmake --preset Release -DVCPKG_TARGET_TRIPLET="${TRIPLET}"
cmake --build --preset Release 
cmake --install build --prefix "${OUT_DIR}" --config Release
# 拷贝 lib 到对应目录
Copy-Item "${OUT_DIR}/lib/*.lib" "${OUT_DIR}/Release/"
$RELEASE_LIB_FILES = "zlib.lib", "libcurl.lib", "libprotobuf.lib"
foreach ($lib in $RELEASE_LIB_FILES) {
  Copy-Item "${VCPKG_TRIPLET_DIR}/lib/${lib}" "${OUT_DIR}/Release/${lib}"
}

# 拷贝头文件、exmaple 文件
New-Item -ItemType Directory -Force -Path "${OUT_DIR}/include"
Copy-Item -Recurse "${VCPKG_TRIPLET_DIR}/include/curl" "${OUT_DIR}/include/curl"
Copy-Item -Recurse "${VCPKG_TRIPLET_DIR}/include/google" "${OUT_DIR}/include/google"
Copy-Item "package/windows/*" "${OUT_DIR}/"
Copy-Item "example/example.cpp" "${OUT_DIR}/"
Remove-Item "${OUT_DIR}/lib" -Force -Recurse
Remove-Item "${OUT_DIR}/build" -Force -Recurse

# todo: add vs project file
