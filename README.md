# server 服务器框架
跟着sylar-yin写的服务器框架

## 日志模块

## 配置模块
使用yaml文件进行配置，使用c++的yaml-cpp库对yaml进行解析
### yaml-cpp使用方法
1. 项目地址:  https://github.com/jbeder/yaml-cpp.git
2. 拉取到本地进行安装
```shell
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build && cd build
cmake -D BUILD_SHARED_LIBS=ON .. 
make -j 4
sudo make install 
```
`注`：如果yaml-cpp安装到项目内，make install 步骤可省略，在make之后直接讲build文件夹下的libyaml-cpp.so,libyaml-cpp.so.0.x，libyaml-cpp.so.0.x.0三个动态库文件，拷贝到项目中对应的文件夹中即可
3. 项目中使用，我这里使用的cmake
```cmake
find_package (PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

include_directories(${YAML_CPP_INCLUDE_DIR})
link_libraries(${YAML_CPP_LIBRARIES})

```
