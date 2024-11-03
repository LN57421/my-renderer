#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>

// 构造函数，从文件中加载模型数据
Model::Model(const char *filename)
    : verts_(),
      faces_(),
      norms_(),
      uv_(),
      diffusemap_(),
      normalmap_(),
      specularmap_() {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {  // 读取顶点数据
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 3, "vn ")) {  // 读取法线数据
            iss >> trash >> trash;
            Vec3f n;
            for (int i = 0; i < 3; i++) iss >> n[i];
            norms_.push_back(n);
        } else if (!line.compare(0, 3, "vt ")) {  // 读取纹理坐标数据
            iss >> trash >> trash;
            Vec2f uv;
            for (int i = 0; i < 2; i++) iss >> uv[i];
            uv_.push_back(uv);
        } else if (!line.compare(0, 2, "f ")) {  // 读取面数据
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                for (int i = 0; i < 3; i++)
                    tmp[i]--;  // Wavefront OBJ 的索引从 1 开始，这里减 1
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# 顶点数: " << verts_.size() << " 面数: " << faces_.size()
              << " 纹理坐标数: " << uv_.size() << " 法线数: " << norms_.size()
              << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap_);  // 加载漫反射贴图
    load_texture(filename, "_nm.tga", normalmap_);        // 加载法线贴图
    load_texture(filename, "_spec.tga", specularmap_);    // 加载高光贴图
}

// 析构函数
Model::~Model() {}

// 返回顶点数量
int Model::nverts() { return (int)verts_.size(); }

// 返回面数量
int Model::nfaces() { return (int)faces_.size(); }

// 获取指定面中的顶点索引
std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i = 0; i < (int)faces_[idx].size(); i++)
        face.push_back(faces_[idx][i][0]);
    return face;
}

// 返回指定索引的顶点坐标
Vec3f Model::vert(int i) { return verts_[i]; }

// 获取指定面和顶点的坐标
Vec3f Model::vert(int iface, int nthvert) {
    return verts_[faces_[iface][nthvert][0]];
}

// 加载纹理文件，suffix 为文件后缀（如"_diffuse.tga"）
void Model::load_texture(std::string filename, const char *suffix,
                         TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot != std::string::npos) {
        texfile = texfile.substr(0, dot) + std::string(suffix);
        std::cerr << "纹理文件 " << texfile << " 加载 "
                  << (img.read_tga_file(texfile.c_str()) ? "成功" : "失败")
                  << std::endl;
        img.flip_vertically();
    }
}

// 根据 UV 坐标获取漫反射颜色
TGAColor Model::diffuse(Vec2f uvf) {
    Vec2i uv(uvf[0] * diffusemap_.get_width(),
             uvf[1] * diffusemap_.get_height());
    return diffusemap_.get(uv[0], uv[1]);
}

// 根据 UV 坐标获取法线
Vec3f Model::normal(Vec2f uvf) {
    Vec2i uv(uvf[0] * normalmap_.get_width(), uvf[1] * normalmap_.get_height());
    TGAColor c = normalmap_.get(uv[0], uv[1]);
    Vec3f res;
    for (int i = 0; i < 3; i++) res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
    return res;
}

// 获取指定面和顶点的 UV 坐标
Vec2f Model::uv(int iface, int nthvert) {
    return uv_[faces_[iface][nthvert][1]];
}

// 根据 UV 坐标获取高光强度
float Model::specular(Vec2f uvf) {
    Vec2i uv(uvf[0] * specularmap_.get_width(),
             uvf[1] * specularmap_.get_height());
    return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}

// 获取指定面和顶点的法线
Vec3f Model::normal(int iface, int nthvert) {
    int idx = faces_[iface][nthvert][2];
    return norms_[idx].normalize();
}
