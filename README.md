# my-renderer

一个由C++实现的**零依赖软渲染器**，零依赖代表项目不依赖任何第三方库，软渲染代表所有的计算都是在CPU侧进行

## 1. 项目特征

- [x] 利用画点API，采用Bresenham画线算法，画出直线
- [x] 实现OBJ模型数据读取，画出模型线框 
- [x] 利用画线API，利用线性插值因子，实现画实心三角形
- [x] 根据数据画出模型三角形面，实现Lambertian漫反射和backface culling，以剔除背面三角形

## 2. 项目架构

为了方便后续学习，特别将一些关键算法的实现放于test文件夹下:

- [x] `draw_lines.cpp`: Bresenham 画线算法
- [x] `draw_triangle.cpp`: 一个画空心三角形算法，两个实心三角形算法
- [x] 'wireframe_rendering.cpp': 读取OBJ模型数据，画出模型线框
- [x] 'backface_culling.cpp': 实现Lambertian漫反射和backface culling，剔除背面三角形

## 3. 构建与运行

本项目使用Windows下使用VSCode + MinGW-w64 + CMake开发

要构建本项目，请在**项目的根目录**中运行以下命令，项目会自动进行构建，并将bulid文件放在根目录下面：

```C++
cmake -B build
```

接下来，运行以下命令以编译项目：

```C++
cmake --build build --config Release
```

要运行项目，请在项目的根目录中运行以下命令：


```C++
./output/main.exe
```

## 4. 附录

MinGW-w64配置：

tinyrenderer：



