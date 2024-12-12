# desciption

this is the C++ SDK version 0.6.0 for SLS 

# dependency

curl: use the system curl or your own curl

lz4 : use lib/liblz4.a


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

## using cmake
+ build lib with cmake
   ```bash
   export CURL_INCLUDE_DIR="/usr/include" # replace with your libcurl include dir
   cmake -B build
   cmake --build build
   ```



