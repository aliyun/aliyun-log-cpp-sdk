
# 简介

[English](README.md) | 中文文档  

此仓库包含阿里云日志服务（SLS）的 c++ SDK，当前版本为 v0.6.0。

# 支持的平台与系统

| CPU  | 系统    | 编译器 | 编译状态|
|--------|---------|-------|-----|
| x64 | linux   | gcc |![Ubuntu Build](https://github.com/crimson-gao/aliyun-log-cpp-sdk/actions/workflows/ubuntu-build.yml/badge.svg) |   |
| x64 | windows | msvc | ![Windows Build](https://github.com/crimson-gao/aliyun-log-cpp-sdk/actions/workflows/windows-build.yml/badge.svg)|
| x86    | linux   | gcc | 暂不支持 |
| x86    | windows | msvc | ![Windows Build](https://github.com/crimson-gao/aliyun-log-cpp-sdk/actions/workflows/windows-build.yml/badge.svg) |  


# 构建
## 从预编译库构建（推荐）
可在 release 页面下载对应压缩包，按照压缩包内 README 说明进行构建  
- x64-windows 支持 cmake 构建、visual studio 构建
- x64-linux 支持 cmake 构建、makefile 构建

## 从源码构建
### vcpkg 构建
1. 安装 vcpkg，可参考[教程](https://learn.microsoft.com/zh-cn/vcpkg/get_started/get-started?pivots=shell-cmd)  
- windows下  
  ```bash
  git clone https://github.com/microsoft/vcpkg.git
  cd vcpkg && bootstrap-vcpkg.bat
  ```

- linux 下
  ```bash
  git clone https://github.com/microsoft/vcpkg.git
  cd vcpkg && ./bootstrap-vcpkg.sh
  ```

2. 安装依赖库  
    ```bash
    vcpkg install protobuf curl
    ```

3. 构建  
设置环境变量 VCPKG_ROOT ，然后执行构建命令  

- linux 下
  ```bash
  export VCPKG_ROOT=你的vcpkg安装目录路径
  cmake --preset Release
  cmake --build --preset Release --config Release
  ```

- windows 下
  ```cmd
  set VCPKG_ROOT="你的vcpkg安装目录路径"
  cmake --preset Release
  cmake --build --preset Release --config Release
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

