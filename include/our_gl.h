#ifndef __OUR_GL_H__
#define __OUR_GL_H__
#include "geometry.h"
#include "tgaimage.h"

// 外部声明的全局矩阵，用于存储模型视图、视口和投影变换矩阵
extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;
const float depth = 2000.f;  // 深度范围常量，用于深度缓冲区

// 设置视口矩阵，(x, y) 为视口左下角坐标，w 和 h 为视口宽度和高度
void viewport(int x, int y, int w, int h);

// 设置投影矩阵，coeff 为投影参数（默认值为 0），通常用于透视投影
// coeff = -1/c 表示远裁剪面的距离
void projection(float coeff = 0.f);

// 设置视图矩阵，用于定义相机的位置和方向
// eye 为相机位置，center 为观察目标点，up 为相机的上方向
void lookat(Vec3f eye, Vec3f center, Vec3f up);

// 着色器接口，用于实现顶点和片段着色器的多态接口
struct IShader {
    virtual ~IShader();  // 虚析构函数
    // 顶点着色器接口，iface 为面索引，nthvert 为顶点索引，返回该顶点的坐标
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    // 片段着色器接口，bar 为重心坐标，color 为输出的颜色值，返回是否丢弃该片段
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

// 绘制三角形的函数，pts 是三角形的三个顶点，shader 为使用的着色器，
// image 为输出图像，zbuffer 为深度缓冲区
void triangle(Vec4f *pts, IShader &shader, TGAImage &image, float *zbuffer);

#endif  // __OUR_GL_H__
