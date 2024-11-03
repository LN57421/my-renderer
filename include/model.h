#ifndef __MODEL_H__
#define __MODEL_H__
#include <string>
#include <vector>

#include "geometry.h"
#include "tgaimage.h"

// 模型类，用于加载和操作3D模型
class Model {
private:
    std::vector<Vec3f> verts_;  // 存储模型的顶点
    std::vector<std::vector<Vec3i>>
        faces_;  // 存储模型的面，每个面包含三个整数，表示顶点/UV/法线索引
    std::vector<Vec3f> norms_;  // 存储模型的法线
    std::vector<Vec2f> uv_;     // 存储纹理坐标
    TGAImage diffusemap_;       // 漫反射贴图
    TGAImage normalmap_;        // 法线贴图
    TGAImage specularmap_;      // 高光贴图

    // 加载纹理方法，filename是文件名，suffix是文件后缀（如"_diffuse",
    // "_normal"等），img是加载的图像
    void load_texture(std::string filename, const char *suffix, TGAImage &img);

public:
    // 构造函数，通过文件名加载模型数据
    Model(const char *filename);

    // 析构函数
    ~Model();

    // 返回模型的顶点数量
    int nverts();

    // 返回模型的面数量
    int nfaces();

    // 返回指定面上第 nthvert 个顶点的法线
    Vec3f normal(int iface, int nthvert);

    // 根据纹理坐标返回对应的法线
    Vec3f normal(Vec2f uv);

    // 返回指定顶点的坐标
    Vec3f vert(int i);

    // 返回指定面上第 nthvert 个顶点的坐标
    Vec3f vert(int iface, int nthvert);

    // 返回指定面上第 nthvert 个顶点的纹理坐标
    Vec2f uv(int iface, int nthvert);

    // 根据纹理坐标返回漫反射颜色
    TGAColor diffuse(Vec2f uv);

    // 根据纹理坐标返回高光强度
    float specular(Vec2f uv);

    // 返回指定面中顶点的索引数组
    std::vector<int> face(int idx);
};

#endif  // __MODEL_H__
