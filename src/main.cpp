#include <iostream>
#include <limits>
#include <vector>

#include "geometry.h"
#include "model.h"
#include "our_gl.h"
#include "tgaimage.h"

// 模型指针和阴影缓冲区指针
Model *model = NULL;
float *shadowbuffer = NULL;

const int width = 800;   // 图像宽度
const int height = 800;  // 图像高度

// 光源、视点和观察方向的定义
Vec3f light_dir(1, 1, 0);
Vec3f eye(1, 1, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

// 自定义着色器类，用于实现光影效果
struct Shader : public IShader {
    mat<4, 4, float> uniform_M;        // 投影和模型视图矩阵的组合
    mat<4, 4, float> uniform_MIT;      // (投影*模型视图)的逆转置，用于法线变换
    mat<4, 4, float> uniform_Mshadow;  // 从帧缓冲区到阴影缓冲区的变换矩阵
    mat<2, 3, float>
        varying_uv;  // 三角形的 UV 坐标，由顶点着色器写入，片段着色器读取
    mat<3, 3, float> varying_tri;  // 三角形顶点坐标，在 Viewport
                                   // 变换前，由顶点着色器写入，片段着色器读取

    // 构造函数初始化矩阵
    Shader(Matrix M, Matrix MIT, Matrix MS)
        : uniform_M(M),
          uniform_MIT(MIT),
          uniform_Mshadow(MS),
          varying_uv(),
          varying_tri() {}

    // 顶点着色器，计算顶点的屏幕坐标
    virtual Vec4f vertex(int iface, int nthvert) {
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = Viewport * Projection * ModelView *
                          embed<4>(model->vert(iface, nthvert));
        varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    // 片段着色器，计算当前片段的颜色
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec4f sb_p = uniform_Mshadow *
                     embed<4>(varying_tri * bar);  // 阴影缓冲区中的对应点
        sb_p = sb_p / sb_p[3];
        int idx = int(sb_p[0]) + int(sb_p[1]) * width;  // 阴影缓冲区数组索引
        //  在计算 shadow 系数时增加一个深度偏移量
        float bias = 43.34;  // 偏移量大小可以调整，根据场景和视角需要微调
                             // 阴影系数，避免 Z fighting
        float shadow =
            .3 + .7 * (shadowbuffer[idx] < (sb_p[2] + bias));  // 加入偏移量

        Vec2f uv = varying_uv * bar;  // 当前像素的 UV 插值
        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv)))
                      .normalize();  // 法线
        Vec3f l =
            proj<3>(uniform_M * embed<4>(light_dir)).normalize();  // 光照向量
        Vec3f r = (n * (n * l * 2.f) - l).normalize();             // 反射光线
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
        float diff = std::max(0.f, n * l);
        TGAColor c = model->diffuse(uv);
        for (int i = 0; i < 3; i++)
            color[i] = std::min<float>(
                20 + c[i] * shadow * (1.6 * diff + .6 * spec), 255);
        return false;
    }
};

// 深度着色器类，用于计算深度缓冲区
struct DepthShader : public IShader {
    mat<3, 3, float> varying_tri;

    DepthShader() : varying_tri() {}

    // 顶点着色器，计算顶点的屏幕坐标
    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;
        varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    // 片段着色器，计算当前片段的深度值
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec3f p = varying_tri * bar;
        color = TGAColor(255, 255, 255) * (p.z / depth);
        return false;
    }
};

int main(int argc, char **argv) {
    float *zbuffer = new float[width * height];
    shadowbuffer = new float[width * height];
    for (int i = width * height; --i;) {
        zbuffer[i] = shadowbuffer[i] = -std::numeric_limits<float>::max();
    }

    model = new Model("obj/african_head/african_head.obj");  // 加载模型
    light_dir.normalize();

    {  // 渲染阴影缓冲区
        TGAImage depth(width, height, TGAImage::RGB);
        lookat(light_dir, center, up);
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        projection(0);

        DepthShader depthshader;
        Vec4f screen_coords[3];
        for (int i = 0; i < model->nfaces(); i++) {
            for (int j = 0; j < 3; j++) {
                screen_coords[j] = depthshader.vertex(i, j);
            }
            triangle(screen_coords, depthshader, depth, shadowbuffer);
        }
        depth.flip_vertically();
        depth.write_tga_file("depth.tga");
    }

    Matrix M = Viewport * Projection * ModelView;

    {  // 渲染帧缓冲区
        TGAImage frame(width, height, TGAImage::RGB);
        lookat(eye, center, up);
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        projection(-1.f / (eye - center).norm());

        Shader shader(ModelView, (Projection * ModelView).invert_transpose(),
                      M * (Viewport * Projection * ModelView).invert());
        Vec4f screen_coords[3];
        for (int i = 0; i < model->nfaces(); i++) {
            for (int j = 0; j < 3; j++) {
                screen_coords[j] = shader.vertex(i, j);
            }
            triangle(screen_coords, shader, frame, zbuffer);
        }
        frame.flip_vertically();
        frame.write_tga_file("framebuffer.tga");
    }

    delete model;
    delete[] zbuffer;
    delete[] shadowbuffer;
    return 0;
}
