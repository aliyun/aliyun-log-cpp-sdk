# desciption

this is the C++ SDK version 0.6.0 for SLS

# dependency

protobuf: require version 2.4.1 ,
install protoc on your machine or use lib/libprotobuf_static.a

curl: use the system curl or your own curl

lz4 : use lib/liblz4.a

# build

## using make file

- execute "make" in this dir, then get
  ```
     lib/libslssdk.a
     lib/libsls_logs_pb.a
     lib/liblz4.a
  ```
- build your own program

```
        g++ -o your_program your_program.o   -O2 -L./lib/   -lslssdk -llz4 -lcurl -lprotobuf

```

## using SConscript

- suppose your working directory is $root

- mkdir $root/slssdk

- copy

  ```
  adapter.cpp  client.cpp  common.cpp  resource.cpp signer.cpp
  adapter.h  client.h  common.h  resource.h  signer.h  RestfulApiCommon.h
  sls_logs.proto
  lz4  rapidjson
  ```

  to $root/slssdk

- build sls sdk lib

```
env.aProto('sls_logs.proto')
env.aStaticLibrary(target = 'sls_logs_pb_cpp', source=['sls_logs.pb.cc'])

slssdk_obj = env.Object([Glob('*.cpp')])
env.aStaticLibrary(target = 'slssdk' ,source = [slssdk_obj])
```

- copy

```
lib/liblz4.a lib/libprotobuf_static.a
```

to $root/slssdk/

- build your own program

```
env.aProgram(target= 'sample1' ,source=['sample.cpp'], LIBS=['slssdk','sls_logs_pb_cpp','lz4','curl','protobuf_static'])
```

## using Cmake

Protobuf and curl are required.

- build lib slssdk

```
mkdir -p build && cd build
cmake .. && make
```

> The target files are in `build/bin` and `build/lib`

- build sample program (optional)

```
mkdir -p build && cd build
cmake .. -DENABLE_BUILD_SAMPLE=ON && make
```

- install libs and header files

```
make install
```

- build your own program

```
 g++ -o your_program your_program.o -O2 -lslssdk
```

# test

The unittest uses [doctest](https://github.com/doctest/doctest) from github.

## using CMake

```
mkdir -p build && cd build
cmake .. -DENABLE_UNITTEST=ON
make

```
