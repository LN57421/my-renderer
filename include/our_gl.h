#include "geometry.h"
#include "tgaimage.h"

// 定义视口矩阵，指定渲染图像在屏幕上的位置和尺寸
void viewport(const int x, const int y, const int w, const int h);

// 定义投影矩阵，用于将3D坐标投影到2D平面上
void projection(const double coeff = 0);  // coeff = -1/c，其中 c 是相机距离

// 定义模型视图矩阵，用于从相机位置观察场景
void lookat(const vec3 eye, const vec3 center, const vec3 up);

// 着色器接口类，提供片段着色器（fragment shader）的抽象接口
struct IShader {
    // 静态方法，用于从纹理中采样颜色
    static TGAColor sample2D(const TGAImage &img, vec2 &uvf) {
        return img.get(uvf[0] * img.width(),
                       uvf[1] * img.height());  // 根据 uv 坐标获取纹理颜色
    }
    // 纯虚函数 fragment，需在子类中实现，用于计算每个片段的颜色
    virtual bool fragment(const vec3 bar, TGAColor &color) = 0;
};

// 渲染三角形，将三角形的每个像素逐个填充到图像中
void triangle(const vec4 clip_verts[3], IShader &shader, TGAImage &image,
              std::vector<double> &zbuffer);
