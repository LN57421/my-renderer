# my-renderer

## 0. 项目简介

此项目为加深对于openGL等图形学API底层渲染理解。这是一个由C++实现的仿openGL的**零依赖软渲染器**，零依赖代表项目不依赖任何第三方库，软渲染代表所有的计算都是在CPU侧进行

## 1. 项目特征

- [x] Bresenham画线算法：利用画点API，采用Bresenham画线算法，画出直线
- [x] wireframe-drawing：实现OBJ模型数据读取，画出模型线框 
- [x] trangile-drawing：利用画线API，利用线性插值因子，实现画实心三角形
- [x] Lambertian漫反射：实现Lambertian漫反射用于光照计算
- [x] backface culling：根据数据画出模型三角形面，并剔除背面三角形
- [x] z-buffer：计算像素深度缓冲, 实现z-testing
- [x] barycentric: 质心计算, 用于后续实现三角形内光照，纹理插值 （颜色，法向量，深度插值）
- [x] perspective projection：透视投影，实现模型的透视投影
- [x] texture-mapping：实现模型的纹理映射
- [x] Gouraud shading：实现模型的Gouraud shading
- [x] glulookat：实现模型的MV矩阵变换
- [ ] Phong shading：单独封装顶点着色器和片元着色器，实现模型的Phong shading


## 2. 项目架构

## include
- `geometry.h`
- `model.h`
- `tgaimage.h`

### obj
- `african_head_diffuse.tga`
- `african_head_nm.tga`
- `african_head_read_me.txt`
- `african_head_spec.tga`
- `african_head_SSS.jpg`
- `african_head.obj`
- `cube.obj`

## output

## src
- `geometry.cpp`
- `main.cpp`
- `model.cpp`
- `tgaimage.cpp`

## test
- `backface_culling.cpp`
- `barycentric.cpp`
- `draw_lines.cpp`
- `draw_triangles.cpp`
- `glLookAt_GouraudShading.cpp`
- `perspective_projection.cpp`
- `wireframe_rendering.cpp`

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

## 4. 参考

MinGW-w64配置：https://zhuanlan.zhihu.com/p/610895870

tinyrenderer：https://github.com/ssloy/tinyrenderer/wiki/



