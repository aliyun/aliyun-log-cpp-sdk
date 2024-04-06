
# Windows 上使用 CMake 构建

## Debug 模式
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
Copy-Item build/Debug/example.exe Debug/
./Debug/example.exe
```

## Release 模式

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
Copy-Item build/Release/example.exe Release/
./Release/example.exe
```


# Windows 上使用 Visual Studio 构建
## 构建样例程序
使用 Visual Studio 打开此目录，在解决方案资源管理器中双击 SlsExample.sln，点击`调试-开始调试`，即可启动样例程序

## 引入到已存在的 vs 项目
### Debug 模式
1. 将 include 文件夹添加到项目属性的`VC++目录-包含目录`中  
2. 将 Debug 目录添加到项目属性 Debug 配置下的`VC++目录-库目录`  
3. 将 `aliyun_log.lib;libcurl.lib;zlib.lib;libprotobuf.lib;Crypt32.lib` 添加到项目属性 Debug 配置下的 `链接器-输入-附加依赖项`  
4. 将 Debug 目录下的 dll 文件拷贝到可执行程序目录下，或者在`项目属性-生成事件-生成后事件-命令行中`添加
```powershell
XCOPY "$(SolutionDir)$(Configuration)"\*.dll "$(TargetDir)" /D /K /Y
```

### Release 模式
1. 将 include 文件夹添加到项目属性的`VC++目录-包含目录`中  
2. 将 Release 目录添加到项目属性 Release 配置下的`VC++目录-库目录`  
3. 将 `aliyun_log.lib;libcurl.lib;zlib.lib;libprotobuf.lib;Crypt32.lib` 添加到项目属性 Release 配置下的 `链接器-输入-附加依赖项`  
4. 将 Release 目录下的 dll 文件拷贝到可执行程序目录下，或者在`项目属性-生成事件-生成后事件-命令行中`添加
```powershell
XCOPY "$(SolutionDir)$(Configuration)"\*.dll "$(TargetDir)" /D /K /Y
```
