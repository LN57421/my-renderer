#include "model.h"

#include <iostream>
#include <sstream>

// 构造函数，从文件加载模型数据
Model::Model(const std::string filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);  // 打开文件
    if (in.fail())
        return;  // 如果文件打开失败，直接返回
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);  // 逐行读取文件
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {  // 处理顶点 "v " 行
            iss >> trash;
            vec3 v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts.push_back(v);
        } else if (!line.compare(0, 3, "vn ")) {  // 处理法线 "vn " 行
            iss >> trash >> trash;
            vec3 n;
            for (int i = 0; i < 3; i++) iss >> n[i];
            norms.push_back(n.normalized());
        } else if (!line.compare(0, 3, "vt ")) {  // 处理纹理坐标 "vt " 行
            iss >> trash >> trash;
            vec2 uv;
            for (int i = 0; i < 2; i++) iss >> uv[i];
            tex_coord.push_back({uv.x, 1 - uv.y});  // 将 v 坐标反转
        } else if (!line.compare(0, 2, "f ")) {     // 处理面 "f " 行
            int f, t, n;
            iss >> trash;
            int cnt = 0;
            while (iss >> f >> trash >> t >> trash >>
                   n) {  // 逐个读取顶点、纹理和法线索引
                facet_vrt.push_back(--f);
                facet_tex.push_back(--t);
                facet_nrm.push_back(--n);
                cnt++;
            }
            if (3 != cnt) {  // 确保面是三角形
                std::cerr
                    << "Error: the obj file is supposed to be triangulated"
                    << std::endl;
                return;
            }
        }
    }
    // 输出加载信息
    std::cerr << "# v# " << nverts() << " f# " << nfaces() << " vt# "
              << tex_coord.size() << " vn# " << norms.size() << std::endl;
    // 加载纹理
    load_texture(filename, "_diffuse.tga", diffusemap);
    load_texture(filename, "_nm_tangent.tga", normalmap);
    load_texture(filename, "_spec.tga", specularmap);
}

// 返回顶点数量
int Model::nverts() const { return verts.size(); }

// 返回面数量
int Model::nfaces() const { return facet_vrt.size() / 3; }

// 返回指定索引的顶点
vec3 Model::vert(const int i) const { return verts[i]; }

// 返回指定面的第 nth 个顶点位置
vec3 Model::vert(const int iface, const int nthvert) const {
    return verts[facet_vrt[iface * 3 + nthvert]];
}

// 加载纹理图像
void Model::load_texture(std::string filename, const std::string suffix,
                         TGAImage &img) {
    size_t dot = filename.find_last_of(".");
    if (dot == std::string::npos)
        return;  // 如果找不到文件后缀，直接返回
    std::string texfile = filename.substr(0, dot) + suffix;
    std::cerr << "texture file " << texfile << " loading "
              << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed")
              << std::endl;
}

// 从法线贴图中获取法线向量
vec3 Model::normal(const vec2 &uvf) const {
    TGAColor c =
        normalmap.get(uvf[0] * normalmap.width(), uvf[1] * normalmap.height());
    return vec3{(double)c[2], (double)c[1], (double)c[0]} * 2. / 255. -
           vec3{1, 1, 1};  // 转换到 [-1, 1] 范围
}

// 返回指定面的第 nth 个顶点的纹理坐标
vec2 Model::uv(const int iface, const int nthvert) const {
    return tex_coord[facet_tex[iface * 3 + nthvert]];
}

// 返回指定面的第 nth 个顶点的法线
vec3 Model::normal(const int iface, const int nthvert) const {
    return norms[facet_nrm[iface * 3 + nthvert]];
}
