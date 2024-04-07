
$OUT_DIR="out"
$target=$args[0]
$VCPKG_ROOT=$env:VCPKG_ROOT

if ($target -ne "All" -and $target -ne "Debug" -and $target -ne "Release") {
  Write-Host "Usage: .\pkg.ps1 [all|Debug|Release]"
  Write-Host "target is $target"
  exit -1
}

function rmdir-if($dir) {
  if (Test-Path $dir) {
    Remove-Item -Recurse -Force $dir
  }
}

# 清空目录
rmdir-if ${OUT_DIR}
New-Item -ItemType Directory -Force -Path ${OUT_DIR}

# 默认值 x64-windows-static-md
$TRIPLET = If ($null -ne ${VCPKG_TARGET_TRIPLET}) {${VCPKG_TARGET_TRIPLET}} Else {"x64-windows"}
$VCPKG_TRIPLET_DIR = "${VCPKG_ROOT}/installed/${TRIPLET}"
Write-Host "VCPKG_ROOT: ${VCPKG_ROOT}"
Write-Host "TRIPLET: ${TRIPLET}"

# ==== 构建 Debug 版本 ==== 
if ($target -eq "All" -or $target -eq "Debug") {
  Write-Host "build on windows Debug"
  rmdir-if build
  New-Item -ItemType Directory -Force -Path "${OUT_DIR}/Debug"
  cmake --preset Debug -DVCPKG_TARGET_TRIPLET="${TRIPLET}" ; if ($LASTEXITCODE -ne 0) { exit -1 }
  cmake --build --preset Debug  ; if ($LASTEXITCODE -ne 0) { exit -2 }
  cmake --install build --prefix "${OUT_DIR}" --config Debug ; if ($LASTEXITCODE -ne 0) { exit -1 }
  # 拷贝 lib 到对应目录 
  Copy-Item "${OUT_DIR}/lib/*.lib" "${OUT_DIR}/Debug/"
  Copy-Item "${VCPKG_TRIPLET_DIR}/debug/lib/zlibd.lib" "${OUT_DIR}/Debug/zlib.lib"
  Copy-Item "${VCPKG_TRIPLET_DIR}/debug/lib/libcurl-d.lib" "${OUT_DIR}/Debug/libcurl.lib"
  Copy-Item "${VCPKG_TRIPLET_DIR}/debug/lib/libprotobufd.lib" "${OUT_DIR}/Debug/libprotobuf.lib"
  if (Test-Path "${VCPKG_TRIPLET_DIR}/debug/bin") {
    $DEBUG_DLL_FILES = "zlibd1.dll", "libcurl-d.dll", "libprotobufd.dll"
    foreach ($lib in $DEBUG_DLL_FILES) {
      Copy-Item "${VCPKG_TRIPLET_DIR}/debug/bin/${lib}" "${OUT_DIR}/Debug/${lib}"
    }
  }
  # patch aliyun_log.pdb
  if (Test-path "build/src/Debug/aliyun_log.pdb" -PathType leaf) {
    Copy-Item "build/src/Debug/aliyun_log.pdb" "${OUT_DIR}/Debug/"
  }
}

# ==== 构建 Release 版本 ====
if ($target -eq "All" -or $target -eq "Release") {
  Write-Host "build on windows Release"
  # rmdir-if build
  New-Item -ItemType Directory -Force -Path "${OUT_DIR}/Release"
  cmake --preset Release -DVCPKG_TARGET_TRIPLET="${TRIPLET}" ; if ($LASTEXITCODE -ne 0) { exit -1 }
  cmake --build --preset Release ; if ($LASTEXITCODE -ne 0) { exit -2 }
  cmake --install build --prefix "${OUT_DIR}" --config Release ; if ($LASTEXITCODE -ne 0) { exit -3 }
  # 拷贝 lib 到对应目录
  Copy-Item "${OUT_DIR}/lib/*.lib" "${OUT_DIR}/Release/"
  $RELEASE_LIB_FILES = "zlib.lib", "libcurl.lib", "libprotobuf.lib"
  foreach ($lib in $RELEASE_LIB_FILES) {
    Copy-Item "${VCPKG_TRIPLET_DIR}/lib/${lib}" "${OUT_DIR}/Release/${lib}"
  }
  if (Test-Path "${VCPKG_TRIPLET_DIR}/bin") {
    $RELEASE_DLL_FILES = "zlib1.dll", "libcurl.dll", "libprotobuf.dll"
    foreach ($lib in $RELEASE_DLL_FILES) {
      Copy-Item "${VCPKG_TRIPLET_DIR}/bin/${lib}" "${OUT_DIR}/Release/${lib}"
    }
  }
}

# 拷贝头文件、exmaple 文件
New-Item -ItemType Directory -Force -Path "${OUT_DIR}/include"
New-Item -ItemType Directory -Force -Path "${OUT_DIR}/include/sls"
Copy-Item -Recurse "${VCPKG_TRIPLET_DIR}/include/curl" "${OUT_DIR}/include/curl"
Copy-Item -Recurse "${VCPKG_TRIPLET_DIR}/include/google" "${OUT_DIR}/include/google"
Copy-Item "package/windows/*" "${OUT_DIR}/"
Copy-Item "example/example.cpp" "${OUT_DIR}/"
rmdir-if "${OUT_DIR}/lib"
rmdir-if "${OUT_DIR}/bin"

# todo: add vs project file
