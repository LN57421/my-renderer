#include <limits>

#include "model.h"
#include "our_gl.h"

// 输出图像尺寸
constexpr int width = 800;
constexpr int height = 800;
// 光源方向
constexpr vec3 light_dir{1, 1, 1};
// 相机位置
constexpr vec3 eye{1, 1, 3};
// 相机观察的中心方向
constexpr vec3 center{0, 0, 0};
// 相机的上方向
constexpr vec3 up{0, 1, 0};

// 外部定义的“OpenGL”状态矩阵
extern mat<4, 4> ModelView;
extern mat<4, 4> Projection;

// 着色器结构体，实现了 IShader 接口
struct Shader : IShader {
    const Model &model;  // 3D模型对象
    vec3 uniform_l;      // 视图坐标系中的光源方向
    mat<2, 3>
        varying_uv;  // 三角形的纹理坐标，在顶点着色器中设置，在片段着色器中使用
    mat<3, 3> varying_nrm;  // 每个顶点的法线，在片段着色器中插值使用
    mat<3, 3> view_tri;     // 视图坐标系下的三角形顶点位置

    // 构造函数，传入模型对象
    Shader(const Model &m) : model(m) {
        // 将光源方向转换到视图坐标系
        uniform_l = proj<3>((ModelView * embed<4>(light_dir, 0.))).normalized();
    }

    // 顶点着色器
    virtual void vertex(const int iface, const int nthvert, vec4 &gl_Position) {
        // 获取并设置三角形顶点的纹理坐标
        varying_uv.set_col(nthvert, model.uv(iface, nthvert));
        // 获取并设置三角形顶点的法线向量，转换到视图坐标系
        varying_nrm.set_col(
            nthvert, proj<3>((ModelView).invert_transpose() *
                             embed<4>(model.normal(iface, nthvert), 0.)));
        // 计算视图坐标下的顶点位置
        gl_Position = ModelView * embed<4>(model.vert(iface, nthvert));
        view_tri.set_col(nthvert, proj<3>(gl_Position));
        // 应用投影矩阵，得到齐次裁剪坐标
        gl_Position = Projection * gl_Position;
    }

    // 片段着色器
    virtual bool fragment(const vec3 bar, TGAColor &gl_FragColor) {
        // 通过重心坐标插值得到片段法线和纹理坐标
        vec3 bn = (varying_nrm * bar).normalized();  // 插值得到片段法线
        vec2 uv = varying_uv * bar;                  // 插值得到纹理坐标

        // 计算切线空间的基底矩阵，用于法线映射
        mat<3, 3> AI = mat<3, 3>{
            {view_tri.col(1) - view_tri.col(0),
             view_tri.col(2) - view_tri.col(0),
             bn}}.invert();
        vec3 i = AI * vec3{varying_uv[0][1] - varying_uv[0][0],
                           varying_uv[0][2] - varying_uv[0][0], 0};
        vec3 j = AI * vec3{varying_uv[1][1] - varying_uv[1][0],
                           varying_uv[1][2] - varying_uv[1][0], 0};
        mat<3, 3> B =
            mat<3, 3>{{i.normalized(), j.normalized(), bn}}.transpose();

        // 从法线贴图中获取法线，并将其从切线空间转换到视图空间
        vec3 n = (B * model.normal(uv)).normalized();
        // 计算漫反射强度，法线与光源方向的点积
        double diff = std::max(0., n * uniform_l);
        // 计算反射光方向，用于高光计算
        vec3 r = (n * (n * uniform_l) * 2 - uniform_l).normalized();
        // 计算高光强度，根据反射光方向和相机方向的夹角
        double spec =
            std::pow(std::max(-r.z, 0.), 5 + sample2D(model.specular(), uv)[0]);

        // 从漫反射贴图中采样颜色
        TGAColor c = sample2D(model.diffuse(), uv);
        // 结合环境光、漫反射和高光，计算片段最终颜色
        for (int i : {0, 1, 2})
            gl_FragColor[i] = std::min<int>(10 + c[i] * (diff + spec),
                                            255);  // 10 表示少量的环境光

        return false;  // 返回 false 表示该片段不被丢弃
    }
};

int main(int argc, char **argv) {
    // 加载模型文件
    Model model("obj/african_head/african_head.obj");

    // 创建输出图像帧缓冲区
    TGAImage framebuffer(width, height, TGAImage::RGB);
    lookat(eye, center, up);  // 设置 ModelView 矩阵
    viewport(width / 8, height / 8, width * 3 / 4,
             height * 3 / 4);           // 设置 Viewport 矩阵
    projection((eye - center).norm());  // 设置 Projection 矩阵
    std::vector<double> zbuffer(
        width * height, std::numeric_limits<double>::max());  // 初始化 Z 缓冲区

    // 为模型的每个面（三角形）调用顶点和片段着色器进行渲染
    Shader shader(model);                       // 创建着色器
    for (int i = 0; i < model.nfaces(); i++) {  // 遍历每个三角形面
        vec4 clip_vert[3];                      // 三角形顶点的齐次裁剪坐标
        for (int j : {0, 1, 2})
            shader.vertex(i, j, clip_vert[j]);  // 调用顶点着色器
        triangle(clip_vert, shader, framebuffer,
                 zbuffer);  // 调用三角形光栅化函数
    }

    // 将帧缓冲区中的图像写入文件
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
