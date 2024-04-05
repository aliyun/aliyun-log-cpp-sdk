# 使用 CMake 构建
## Debug 模式
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
./build/example
```

## Release 模式
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/example
```

# 使用 Makefile 构建
## Debug 模式
```bash
MODE=Debug make -j
./example
```

## Release 模式
```bash
make -j
./example
```
