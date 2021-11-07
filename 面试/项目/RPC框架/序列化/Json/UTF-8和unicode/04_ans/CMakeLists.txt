#cmakelist中，所有的相关语法命令是大小写不敏感的

#CMake最低版本号要求
cmake_minimum_required(VERSION 2.8)

#指定项目名称
project(mujson_test01)

# 设置编译器
set(CMAKE_CXX_COMPILER "g++")

# 设在c++11及开启所有警告
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

# 当编译类型为Debug时，不优化、开启调试信息、针对gdb生成更详细的调试信息
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb")

add_library(mujson mujson.cpp)

add_executable(mujson_test test.cpp)

target_link_libraries(mujson_test mujson)


