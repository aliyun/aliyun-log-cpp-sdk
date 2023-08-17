# Description

English | [中文文档](README_zh.md)  

This is the C++ SDK version 0.6.0 for SLS  

# Supported Platforms
| CPU  | OS    | Compiler | Status|
|--------|---------|-------|-----|
| x64 | linux   | gcc |![Ubuntu Build](https://github.com/crimson-gao/aliyun-log-cpp-sdk/actions/workflows/ubuntu-build.yml/badge.svg) |   |
| x64 | windows | msvc | ![Windows Build](https://github.com/crimson-gao/aliyun-log-cpp-sdk/actions/workflows/windows-build.yml/badge.svg)|
| x86    | linux   | gcc | Not Supported |
| x86    | windows | msvc | ![Windows Build](https://github.com/crimson-gao/aliyun-log-cpp-sdk/actions/workflows/windows-build.yml/badge.svg) |  



# Dependencies

- protobuf: require version 2.4.1.

- protoc  
  `protoc` is a command-line tool.   
  You can download pre-compiled binary from [protobuf release page](https://github.com/protocolbuffers/protobuf/releases), or install `protobuf-compiler` by package management tools.  
    > The version of protobuf and protoc should be matched.   

- curl

- lz4

## Install Dependencies
Third-party libraries can be installed from source code, or using package management tools.   
It is recommended to use apt on ubuntu, yum on centOS, 
and [vcpkg](https://github.com/microsoft/vcpkg) on windows.  

If any package not found, you can search and download it from official website of the distribution system. 

Ubuntu      

```bash
sudo apt install libprotobuf-dev protobuf-compiler liblz4-dev libcurl4-openssl-dev cmake
```

CentOS  

```bash  
sudo yum install protobuf-devel protobuf-compiler lz4-devel libcurl-devel cmake
```


Windows   

>The version of the library to be installed can be specified by suffix. Using `--triplet x86-windows` for the 32-bit version, and `--triplet x64-windows` for the 64-bit version.  
```bash
vcpkg install --triplet x64-windows protobuf lz4 curl
```



# Build
## Support Build Tools
1. Makefile (gcc only)
2. CMake (gcc and msvc) with generators (Makefiles and MsBuild)
## Notice
> The command-line tool protoc will be used to read the definition from file [sls_logs.proto](sls_logs.proto), and generate the corresponding header file (sls_logs.pb.h) and source file (sls_logs.pb.cc).   
These files will be automatically updated everytime the proto file updated, please do not modify them manually.

## Using CMake (linux and windows)
1. Use CMake to configure project, by executing commands below.  


```bash  
mkdir build
cmake -B build
```

If there are some header files or libraries can not be found when using
vcpkg as your package manager, add `-DCMAKE_TOOLCHAIN_FILE=C:/example/vcpkg/scripts/buildsystems/vcpkg.cmake` to the command. Replace `C:/example/vcpkg` with the path of your directory vcpkg installed. 

```bash
mkdir build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/example/vcpkg/scripts/buildsystems/vcpkg.cmake
```

For 32-bit target and compiler msvc on windows    
```
mkdir build
cmake -B build -A Win32 -DCMAKE_GENERATOR_PLATFORM=Win32 -DCMAKE_TOOLCHAIN_FILE=C:/example/vcpkg/scripts/buildsystems/vcpkg.cmake
```

2. Build libraries  

Use the commands below.

```bash  
cmake --build build
```

There must be at least one [generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) installed on your machine, that CMake supports, such as `Unix Makefiles`, `Ninja`, `xcode` or `Visual Studio`.  
You can specify which generator to use by option `-G <generator>`.  

## Using Makefile (gcc on linux only)

1. Execute command `make -j` from the project root directory to generate the following files.   

```
lib/libslssdk.a 
sls_logs.pb.cc
sls_logs.pb.h
sample
```
`lib/libslssdk.a` is the SDK library file that can be statically linked into your program.  
`sample` is a demo executable binary file.

2. Build your program.   
Add the directories of header files and libraries of SLS SDK, and execute the commands below.  

```bash
g++ -o your_program your_program.o   -O2 -L./lib/  -I./include/ -lslssdk -llz4 -lcurl -lprotobuf 
```
 
## Using SConscript(deprecated)

+ suppose your working directory is $root

+ mkdir $root/slssdk

+ copy 
    ```
    adapter.cpp  client.cpp  common.cpp  resource.cpp  
    adapter.h  client.h  common.h  resource.h  RestfulApiCommon.h 
    sls_logs.proto  
    include
    ```
    to $root/slssdk

+ build sls sdk lib 

```
env.aProto('sls_logs.proto')
env.aStaticLibrary(target = 'sls_logs_pb_cpp', source=['sls_logs.pb.cc'])

slssdk_obj = env.Object([Glob('*.cpp')])
env.aStaticLibrary(target = 'slssdk' ,source = [slssdk_obj])
```

+ copy 
```
lib/liblz4.a lib/libprotobuf_static.a
```
to $root/slssdk/

+  build your own program

```
env.aProgram(target= 'sample1' ,source=['sample.cpp'], LIBS=['slssdk','sls_logs_pb_cpp','lz4','curl','protobuf_static'])
```

# Sample Code
See [sample.cpp](example/sample.cpp) for more details.  

1. To put logs to sls, we need to initialize a client object. The input parameters include your AccessKeyID, AccessKeySecret, and the endpoint address of SLS service which depends on the region you access to.
> AccessKeyID, AccessKeySecret are used as your credential to access SLS service. It is highly recommended to use environment variables or command line args to get the arguments. 

```cpp
#include "client.h"
#include <string>
#include <memory>
using namespace aliyun_log_sdk_v6;
using namespace std;
int main()
{
    string endpoint = "cn-hangzhou-corp.sls.aliyuncs.com";
    string accessKeyId = "";
    string accessKeySecret = "";
    string myIp = "127.0.0.1";
    auto clientPtr = make_shared<LOGClient>(endpoint, accessKeyId, accessKeySecret, LOG_REQUEST_TIMEOUT, myIp, false); 
}
```

2. Construct the content of logs, and send it by the client.
```cpp
#include "client.h"
#include <string>
#include <memory>
#include <vector>
#include "common.h"
using namespace aliyun_log_sdk_v6;
using namespace std;
int main() 
{
  // Omit the content of the first step

  string project = "", logstore = "", topic = "";
  vector<LogItem> logs;
  for (int i = 0; i < 2; ++i)
  {
      logs.push_back(LogItem());
      LogItem& item = logs.back();
      item.timestamp = time(NULL) ;
      item.source = "127.0.0.1";
      item.topic = topic;
      item.data.push_back({"status", "200"});
      item.data.push_back({"latency", "126"});
  }
  try
  {
    auto res = clientPtr->PostLogStoreLogs(project, logstore, topic, logs);
  } 
  catch(LOGException & e)
  {
      cout<<e.GetErrorCode()<<":"<<e.GetMessage() <<endl;
  }

}

```

