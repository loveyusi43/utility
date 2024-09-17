cmake_minimum_required(VERSION 3.22)
project(leetcode)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 递归查找所有源文件和头文件
file(GLOB_RECURSE SOURCES "*.cc")
file(GLOB_RECURSE HEADERS "*.h")

# 添加可执行文件
add_executable(leetcode ${SOURCES} ${HEADERS})

# 设置头文件搜索路径
target_include_directories(leetcode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
