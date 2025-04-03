# 简介

这是 SLS 的 C++ SDK。

中文文档 | [English](README.md)

# 依赖项
- curl：必需，可以使用系统自带的 curl 或你自己安装的 curl 库

- lz4：可选，默认自动从源码构建，你也可以指定成你自己的 lz4 库

# 构建

可以通过 Makefile 或者 CMake 完成构建

## 使用Makefile

1. 执行 `make` 命令来构建库文件 `lib/libslssdk.a`

    ```bash
    make -j
    ```

2. 链接 `lib/libslssdk.a` 文件到你的程序里

    ```bash
    g++ -o your_program your_program.o -O2 -L./lib/  -lslssdk -llz4 -lcurl
    ```

### 依赖项

默认会使用系统安装位置的 curl 库，可以通过以下命令来安装 curl 库

Ubuntu:

```bash
sudo apt install libcurl4-openssl-dev
```

Centos/RHEL:

```bash
sudo yum install libcurl-devel
```

MacOs

```bash
brew install curl
```

如果你要手动指定 curl 库的位置，可以设置变量 `CURL_INCLUDE_DIR` 和 `CURL_LIB_DIR`：

```bash
make CURL_INCLUDE_DIR=/usr/local/include CURL_LIB_DIR=/usr/local/lib
```

## 使用CMake

在下面的命令中，替换变量 `CURL_INCLUDE_DIR` 和 `CURL_LIB_DIR` 为你自己的 curl 库路径

```bash
mkdir -p build && cd build
cmake .. -DCURL_INCLUDE_DIR=/usr/local/include -DCURL_LIB_DIR=/usr/local/lib
```

然后执行构建

```bash
cmake --build .
```
