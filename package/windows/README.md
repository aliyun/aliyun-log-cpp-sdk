
# Windows 上使用 CMake 构建

## Debug 模式
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
./build/Debug/example.exe
```

## Release 模式

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/Release/example.exe
```


# Windows 上使用 Visual Studio 构建
