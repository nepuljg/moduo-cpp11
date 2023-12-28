cmake_minimum_required(VERSION 2.8)
project(Mmoduo)

# 编译成.so动态库
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 设置调试信息
set(CMKAE_CXX_FLAGS("${CMAKE_CXX_FLAGS}") -fPIC)
# 定义参与编译的源文件
aux_source_directory(. SRC_LIST)
# 编译动态库
add_library(Mmoduo SHARED ${SRC_LIST})
# 编译动态库
