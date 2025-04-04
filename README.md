# desciption

this is the C++ SDK version 0.6.0 for SLS 

[中文文档](README_CN.md) | English

# dependency

- curl: required, use the system curl or your own [curl](https://curl.se/docs/install.html)  

- lz4 : optional, build from [source](lz4/lz4.h) as part of this sdk or use your own lz4


# build 

## using makefile

+ execute `make` in this dir to get lib `lib/libslssdk.a` 
    ```bash
    make -j
    ```
+ build your own program

    ```bash
    g++ -o your_program your_program.o -O2 -L./lib/  -lslssdk -llz4 -lcurl
    ```

### dependency

- **curl**

    to use your own curl lib, set `CURL_INCLUDE_DIR` and `CURL_LIB_DIR` as follows
    
    ```bash
    make CURL_INCLUDE_DIR=/usr/local/include CURL_LIB_DIR=/usr/local/lib
    ```
    otherwise, use system curl by installing it to your system.
    
    Ubuntu:
    ```bash
    sudo apt install libcurl4-openssl-dev
    ```
    Centos/RHEL:
    ```bash
    sudo yum install libcurl-devel
    ```

## using cmake

- build lib with cmake

   1. configure
   replace `CURL_INCLUDE_DIR` and `CURL_LIB_DIR` with your own curl lib
   ```bash
   mkdir -p build && cd build
   cmake .. -DCURL_INCLUDE_DIR=/usr/local/include -DCURL_LIB_DIR=/usr/local/lib
   ```

   2. build
   ```bash
   cmake --build .
   ```

### dependency

- **curl**

    `CURL_INCLUDE_DIR` and `CURL_LIB_DIR` is required to build libslssdk as follows
    ```bash
    cmake .. -DCURL_INCLUDE_DIR=/usr/local/include -DCURL_LIB_DIR=/usr/local/lib
    ```



