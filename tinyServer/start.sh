#!/bin/bash

# 检查是否存在构建目录，如果存在则删除
if [ -d "build" ]; then
    echo "正在删除旧的构建目录..."
    rm -rf build
fi

# 重新创建构建目录
echo "重新创建构建目录..."
mkdir build

# 进入构建目录
cd build || exit

# 运行 CMake 配置项目
echo "正在配置项目..."
cmake ..

# 编译项目
echo "正在编译项目..."
make

# 检查编译是否成功
if [ -f tinyServer ]; then
    echo "编译成功！"
    # 提示用户输入端口号
    read -p "按照格式运行：tinyServer port_number: " port_number
    # 运行服务器
    ./tinyServer $port_number
else
    echo "编译失败！"
fi