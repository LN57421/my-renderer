#include "our_gl.h"

#include <cmath>
#include <cstdlib>
#include <limits>

// 全局矩阵，用于模型视图、视口和投影变换
Matrix ModelView;
Matrix Viewport;
Matrix Projection;

// 虚析构函数，为接口 `IShader` 提供一个析构函数
IShader::~IShader() {}

// 设置视口变换矩阵
// (x, y) 是视口的左下角坐标，(w, h) 是视口的宽度和高度
void viewport(int x, int y, int w, int h) {
    Viewport = Matrix::identity();
    Viewport[0][3] = x + w / 2.f;
    Viewport[1][3] = y + h / 2.f;
    Viewport[2][3] = depth / 2.f;
    Viewport[0][0] = w / 2.f;
    Viewport[1][1] = h / 2.f;
    Viewport[2][2] = depth / 2.f;
}

// 设置投影矩阵
// coeff 是投影参数，通常用于透视投影
void projection(float coeff) {
    Projection = Matrix::identity();
    Projection[3][2] = coeff;
}

// 设置模型视图矩阵
// eye 是相机位置，center 是观察目标点，up 是相机的上方向
void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye - center).normalize();
    Vec3f x = cross(up, z).normalize();
    Vec3f y = cross(z, x).normalize();
    ModelView = Matrix::identity();
    for (int i = 0; i < 3; i++) {
        ModelView[0][i] = x[i];
        ModelView[1][i] = y[i];
        ModelView[2][i] = z[i];
        ModelView[i][3] = -center[i];
    }
}

// 计算重心坐标
// A, B, C 是三角形的三个顶点，P 是目标点
Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f s[2];
    for (int i = 2; i--;) {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    // 如果 u[2] 不为 0，则返回重心坐标，否则三角形退化，返回负坐标
    if (std::abs(u[2]) > 1e-2)
        return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    return Vec3f(-1, 1, 1);
}

// 绘制三角形
// pts 是三角形的三个顶点，shader 是使用的着色器，image 是目标图像，zbuffer
// 是深度缓冲区
void triangle(Vec4f *pts, IShader &shader, TGAImage &image, float *zbuffer) {
    // 计算三角形的包围盒
    Vec2f bboxmin(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(),
                  -std::numeric_limits<float>::max());
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
        }
    }

    Vec2i P;
    TGAColor color;
    // 遍历包围盒中的每个像素
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            // 计算 P 点相对于三角形的重心坐标
            Vec3f c = barycentric(proj<2>(pts[0] / pts[0][3]),
                                  proj<2>(pts[1] / pts[1][3]),
                                  proj<2>(pts[2] / pts[2][3]), proj<2>(P));
            float z = pts[0][2] * c.x + pts[1][2] * c.y + pts[2][2] * c.z;
            float w = pts[0][3] * c.x + pts[1][3] * c.y + pts[2][3] * c.z;
            int frag_depth = z / w;
            // 如果在三角形内且当前深度小于 zbuffer 的深度，则渲染
            if (c.x < 0 || c.y < 0 || c.z < 0 ||
                zbuffer[P.x + P.y * image.get_width()] > frag_depth)
                continue;
            bool discard = shader.fragment(c, color);
            if (!discard) {
                zbuffer[P.x + P.y * image.get_width()] = frag_depth;
                image.set(P.x, P.y, color);
            }
        }
    }
}
