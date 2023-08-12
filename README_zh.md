
# 简介

[English](README.md) | 中文文档  

![Ubuntu Build](https://github.com/crimson-gao/aliyun-log-cpp-sdk/actions/workflows/ubuntu-build.yml/badge.svg) | ![Windows Build](https://github.com/crimson-gao/aliyun-log-cpp-sdk/actions/workflows/windows-build.yml/badge.svg)  

此仓库包含阿里云日志服务（SLS）的 c++ SDK，当前版本为 v0.6.0。

# 支持的系统与平台

- gcc (version >= 4.9) on linux  
 
- msvc on windows  

# 依赖的第三方库

- protobuf: 要求版本不低于 2.5.0。  

- protoc  
  protoc 是二进制命令行工具，用于从 proto 文件生成 protobuf 头文件与源文件。  
  可以从 [protobuf 发布页面](https://github.com/protocolbuffers/protobuf/releases) 下载已编译好的二进制文件，或者使用包管理工具安装 `protobuf-compiler`。  

  > protoc 与 protobuf 的版本应该相符，避免兼容性问题。  
- curl

- lz4

## 第三方库安装方式
第三方库可以通过源码编译安装，或者使用包管理工具安装。ubuntu 下推荐使用 apt、centos 推荐使用 yum、windows 推荐使用 [vcpkg](https://github.com/microsoft/vcpkg) 进行安装。  

如果有些包未找到，可以去发行版系统的官方网站搜索并下载安装，或者从源码构建安装。  

Ubuntu      

```bash
sudo apt install libprotobuf-dev protobuf-compiler liblz4-dev libcurl4-openssl-dev cmake
```

CentOS  

```bash  
sudo yum install protobuf-devel protobuf-compiler lz4-devel libcurl-devel cmake
```

Windows系统  

> vcpkg 可以指定要安装的库的版本，例如安装 32位 protobuf 库使用 protobuf:x86-windows，64位版本使用 protobuf:x64-windows 安装。
```bash
vcpkg install protobuf:x64-windows lz4:x64-windows curl:x64-windows
```


# 编译构建
## 支持的构建工具
1. Makefile (支持编译器 gcc)
2. CMake (支持编译器 gcc 和 msvc)
## 编译过程
> 在下面的编译过程中，会利用 protoc 工具从 [sls_logs.proto](sls_logs.proto) 文件中读取定义，并生成相应的头文件(sls_logs.pb.h)与源文件(sls_logs.pb.cc)。  
这些文件会在 proto 定义更新后被自动更新，请不要手动修改这些生成的文件。  


## 使用 CMake 构建（支持linux 与 windows）
1. 使用 CMake 配置项目，执行以下命令 
  
```bash  
mkdir build
cmake -B build
```

若您使用 vcpkg 作为包管理工具，在构建过程中出现找不到第三方库的头文件或库文件，可在cmake配置时添加 `-DCMAKE_TOOLCHAIN_FILE=C:/example/vcpkg/scripts/buildsystems/vcpkg.cmake`，其中 `C:/example/vcpkg` 替换为您的 vcpkg 的实际安装目录路径，如下所示  

```bash
mkdir build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/example/vcpkg/scripts/buildsystems/vcpkg.cmake
```

2. 执行构建  

可使用以下命令执行构建。  

```bash  
cmake --build build
```

您的机器上至少需要安装一种 CMake 支持的 [Generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)，常见的 Generator 包括 `Unix Makefiles` 、`Ninja`、`xcode` 、`Visual Studio` 等，若要指定 Generator 可使用 `-G <generator>` 选项。

## 使用 Makefile 构建（支持linux）

1. 在项目根目录执行命令 `make -j`，自动生成以下文件。

```
lib/libslssdk.a 
sls_logs.pb.cc
sls_logs.pb.h
sample
```
其中 `lib/libslssdk.a` 是构建好的 SDK 库文件，可以被静态连接到你的程序中， sample 是一个示例可执行二进制文件。

2. 构建你的程序。  
在你的程序的构建指令中添加 SLS SDK 的头文件与库，并执行构建指令，如下所示。

```bash
g++ -o your_program your_program.o   -O2 -L./lib/  -I./include/ -lslssdk -llz4 -lcurl -lprotobuf 
```

## 使用 SConscript（不推荐）

+ 项目根目录记作 $root，首先创建目录 `mkdir $root/slssdk`

+ 拷贝以下文件到项目根目录
```
adapter.cpp  client.cpp  common.cpp  resource.cpp  
adapter.h  client.h  common.h  resource.h  RestfulApiCommon.h 
sls_logs.proto  
include
```

+ 构建 sls sdk 库文件

```
env.aProto('sls_logs.proto')
env.aStaticLibrary(target = 'sls_logs_pb_cpp', source=['sls_logs.pb.cc'])

slssdk_obj = env.Object([Glob('*.cpp')])
env.aStaticLibrary(target = 'slssdk' ,source = [slssdk_obj])
```

+ 拷贝以下文件到 $root/slssdk/
```
lib/liblz4.a lib/libprotobuf_static.a
```

+  构建你的程序

```
env.aProgram(target= 'sample1' ,source=['sample.cpp'], LIBS=['slssdk','sls_logs_pb_cpp','lz4','curl','protobuf_static'])
```

# 样例
参考 [sample.cpp](example/sample.cpp) 文件。

1. 以写入日志为例，首先我们需要初始化一个客户端对象，输入参数包括您的 AccessKeyID、AccessKeySecret，要访问的 SLS 服务所在地域的 endpoint 地址。  

> AccessKeyID、AccessKeySecret
是您访问 SLS 服务的凭证，建议使用环境变量、启动参数等方式注入，请不要在代码中明文编码，防止意外泄漏。

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

2. 构造日志内容，并使用客户端对象发送。
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
  // 省略第一步内容...

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

