#include "our_gl.h"

// 全局矩阵，用于存储模型视图、视口和投影矩阵
mat<4, 4> ModelView;
mat<4, 4> Viewport;
mat<4, 4> Projection;

// 定义视口矩阵，将模型坐标转换为屏幕坐标
void viewport(const int x, const int y, const int w, const int h) {
    Viewport = {{{w / 2., 0, 0, x + w / 2.},
                 {0, h / 2., 0, y + h / 2.},
                 {0, 0, 1, 0},
                 {0, 0, 0, 1}}};
}

// 定义投影矩阵，实现透视投影
void projection(
    const double f) {  // 参考：https://en.wikipedia.org/wiki/Camera_matrix
    Projection = {
        {{1, 0, 0, 0}, {0, -1, 0, 0}, {0, 0, 1, 0}, {0, 0, -1 / f, 0}}};
}

// 定义模型视图矩阵，基于相机位置、目标位置和上方向设置相机视角
void lookat(
    const vec3 eye, const vec3 center,
    const vec3
        up) {  // 参考：https://github.com/ssloy/tinyrenderer/wiki/Lesson-5-Moving-the-camera
    vec3 z = (center - eye).normalized();  // 计算相机的前向向量
    vec3 x = cross(up, z).normalized();    // 计算相机的右向向量
    vec3 y = cross(z, x).normalized();     // 计算相机的上向向量
    // 定义旋转矩阵 Minv
    mat<4, 4> Minv = {{{x.x, x.y, x.z, 0},
                       {y.x, y.y, y.z, 0},
                       {z.x, z.y, z.z, 0},
                       {0, 0, 0, 1}}};
    // 定义平移矩阵 Tr
    mat<4, 4> Tr = {{{1, 0, 0, -eye.x},
                     {0, 1, 0, -eye.y},
                     {0, 0, 1, -eye.z},
                     {0, 0, 0, 1}}};
    ModelView = Minv * Tr;  // 组合得到模型视图矩阵
}

// 计算给定点 P 在三角形 tri 中的重心坐标
vec3 barycentric(const vec2 tri[3], const vec2 P) {
    mat<3, 3> ABC = {{embed<3>(tri[0]), embed<3>(tri[1]), embed<3>(tri[2])}};
    if (ABC.det() < 1e-3)
        return {-1, 1, 1};  // 如果三角形退化，生成负坐标，使其被光栅化器丢弃
    return ABC.invert_transpose() * embed<3>(P);
}

// 渲染三角形，将其逐像素填充到图像中，并更新深度缓冲
void triangle(const vec4 clip_verts[3], IShader &shader, TGAImage &image,
              std::vector<double> &zbuffer) {
    // 将三角形顶点转换为屏幕坐标（在透视除法之前）
    vec4 pts[3] = {Viewport * clip_verts[0], Viewport * clip_verts[1],
                   Viewport * clip_verts[2]};
    // 透视除法之后的屏幕坐标
    vec2 pts2[3] = {proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]),
                    proj<2>(pts[2] / pts[2][3])};

    // 计算三角形的包围盒
    int bboxmin[2] = {image.width() - 1, image.height() - 1};
    int bboxmax[2] = {0, 0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::min(bboxmin[j], static_cast<int>(pts2[i][j]));
            bboxmax[j] = std::max(bboxmax[j], static_cast<int>(pts2[i][j]));
        }
    }

    // 使用 OpenMP 并行化循环，加速像素填充
#pragma omp parallel for
    for (int x = std::max(bboxmin[0], 0);
         x <= std::min(bboxmax[0], image.width() - 1); x++) {
        for (int y = std::max(bboxmin[1], 0);
             y <= std::min(bboxmax[1], image.height() - 1); y++) {
            // 计算当前像素 (x, y) 的重心坐标
            vec3 bc_screen = barycentric(
                pts2, {static_cast<double>(x), static_cast<double>(y)});
            vec3 bc_clip = {bc_screen.x / pts[0][3], bc_screen.y / pts[1][3],
                            bc_screen.z / pts[2][3]};
            bc_clip =
                bc_clip /
                (bc_clip.x + bc_clip.y +
                 bc_clip
                     .z);  // 参考：https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
            // 计算片段的深度值
            double frag_depth =
                vec3{clip_verts[0][2], clip_verts[1][2], clip_verts[2][2]} *
                bc_clip;
            // 如果当前像素不在三角形内或被深度缓冲剔除，则跳过
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 ||
                frag_depth > zbuffer[x + y * image.width()])
                continue;
            TGAColor color;
            // 调用片段着色器，如果片段被丢弃则跳过
            if (shader.fragment(bc_clip, color))
                continue;
            // 更新深度缓冲区
            zbuffer[x + y * image.width()] = frag_depth;
            // 设置图像中 (x, y) 处的像素颜色
            image.set(x, y, color);
        }
    }
}
