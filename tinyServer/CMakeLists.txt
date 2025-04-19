cmake_minimum_required(VERSION 3.22)
project(tinyServer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源文件
set(SOURCES
    main.cpp
    http_conn.cpp
)

# 添加头文件目录
include_directories(${CMAKE_SOURCE_DIR})

# 添加可执行文件
add_executable(tinyServer ${SOURCES})

# 链接 pthread 库
target_link_libraries(tinyServer pthread)