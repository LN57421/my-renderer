# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称和语言
project(MyRenderer VERSION 1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 定义包含目录
include_directories(include)

# 定义源目录和输出目录
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(OUTPUT_DIR output)

# 输出调试信息
message(STATUS "Sources: ${SOURCES}")

# 查找源文件
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")

# 添加可执行文件
add_executable(main ${SOURCES})

# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)

# 添加库路径
link_directories(lib)

# 添加自定义构建和运行命令
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target main
    # g++ -ggdb -g -pg -O0 keys
    # COMMAND -ggdb -g -pg -O0
    # -O2
    # COMMAND -O2
    COMMAND ${CMAKE_BINARY_DIR}/main
    DEPENDS main
    COMMENT "Building and running..."
)