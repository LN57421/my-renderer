#include <string>
#include <vector>

#include "geometry.h"
#include "tgaimage.h"

// 模型类，用于加载和存储 3D 模型的顶点、法线、纹理坐标等数据
class Model {
    std::vector<vec3> verts{};      // 顶点数组，存储模型的所有顶点
    std::vector<vec2> tex_coord{};  // 每顶点的纹理坐标数组
    std::vector<vec3> norms{};      // 每顶点的法线数组
    std::vector<int> facet_vrt{};   // 三角形面顶点索引，指向 verts 数组
    std::vector<int> facet_tex{};   // 三角形面纹理坐标索引，指向 tex_coord 数组
    std::vector<int> facet_nrm{};   // 三角形面法线索引，指向 norms 数组
    TGAImage diffusemap{};          // 漫反射颜色贴图
    TGAImage normalmap{};           // 法线贴图，用于细节光照计算
    TGAImage specularmap{};         // 高光贴图，用于控制高光效果
    void load_texture(const std::string filename, const std::string suffix,
                      TGAImage& img);  // 加载纹理的私有函数
public:
    Model(const std::string filename);  // 构造函数，加载指定文件的模型
    int nverts() const;                 // 返回顶点数量
    int nfaces() const;                 // 返回三角形面数量
    vec3 normal(const int iface,
                const int nthvert) const;  // 获取指定面的顶点法线
    vec3 normal(const vec2& uv) const;     // 从法线贴图获取法线，用于法线映射
    vec3 vert(const int i) const;          // 获取指定索引的顶点
    vec3 vert(const int iface,
              const int nthvert) const;  // 获取指定面的顶点位置
    vec2 uv(const int iface,
            const int nthvert) const;  // 获取指定面的顶点的纹理坐标
    const TGAImage& diffuse() const { return diffusemap; }    // 获取漫反射贴图
    const TGAImage& specular() const { return specularmap; }  // 获取高光贴图
};
