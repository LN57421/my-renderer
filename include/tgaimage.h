#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push, 1)
// TGA 文件头结构体，用于存储 TGA 图像的头部信息
struct TGA_Header {
    char idlength;         // 图像 ID 长度
    char colormaptype;     // 调色板类型
    char datatypecode;     // 数据类型码
    short colormaporigin;  // 调色板起始位置
    short colormaplength;  // 调色板长度
    char colormapdepth;    // 调色板深度
    short x_origin;        // 图像 x 坐标起点
    short y_origin;        // 图像 y 坐标起点
    short width;           // 图像宽度
    short height;          // 图像高度
    char bitsperpixel;     // 每像素位数
    char imagedescriptor;  // 图像描述符
};
#pragma pack(pop)

// TGAColor 结构体，用于表示颜色信息
struct TGAColor {
    unsigned char bgra[4];  // 颜色数据，BGRA 格式
    unsigned char bytespp;  // 每像素字节数

    // 默认构造函数，将颜色初始化为黑色
    TGAColor() : bgra(), bytespp(1) {
        for (int i = 0; i < 4; i++) bgra[i] = 0;
    }

    // 使用 RGBA 值初始化颜色
    TGAColor(unsigned char R, unsigned char G, unsigned char B,
             unsigned char A = 255)
        : bgra(), bytespp(4) {
        bgra[0] = B;
        bgra[1] = G;
        bgra[2] = R;
        bgra[3] = A;
    }

    // 灰度颜色构造函数
    TGAColor(unsigned char v) : bgra(), bytespp(1) {
        for (int i = 0; i < 4; i++) bgra[i] = 0;
        bgra[0] = v;
    }

    // 从字节数组和字节数构造颜色
    TGAColor(const unsigned char *p, unsigned char bpp) : bgra(), bytespp(bpp) {
        for (int i = 0; i < (int)bpp; i++) {
            bgra[i] = p[i];
        }
        for (int i = bpp; i < 4; i++) {
            bgra[i] = 0;
        }
    }

    // 下标运算符重载，用于访问颜色分量
    unsigned char &operator[](const int i) { return bgra[i]; }

    // 颜色乘法，按指定强度缩放颜色
    TGAColor operator*(float intensity) const {
        TGAColor res = *this;
        intensity =
            (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
        for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * intensity;
        return res;
    }
};

// TGAImage 类，用于读取、写入和处理 TGA 格式的图像
class TGAImage {
protected:
    unsigned char *data;  // 图像数据指针
    int width;            // 图像宽度
    int height;           // 图像高度
    int bytespp;          // 每像素字节数

    // 加载 RLE 压缩数据
    bool load_rle_data(std::ifstream &in);

    // 卸载 RLE 压缩数据
    bool unload_rle_data(std::ofstream &out);

public:
    // 图像格式枚举类型
    enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

    TGAImage();                       // 默认构造函数
    TGAImage(int w, int h, int bpp);  // 使用宽度、高度和字节数构造图像
    TGAImage(const TGAImage &img);    // 复制构造函数

    // 从文件读取 TGA 图像
    bool read_tga_file(const char *filename);

    // 将图像写入文件，默认启用 RLE 压缩
    bool write_tga_file(const char *filename, bool rle = true);

    // 水平翻转图像
    bool flip_horizontally();

    // 垂直翻转图像
    bool flip_vertically();

    // 缩放图像到指定宽度和高度
    bool scale(int w, int h);

    // 获取指定位置的颜色
    TGAColor get(int x, int y);

    // 设置指定位置的颜色，非常量版本
    bool set(int x, int y, TGAColor &c);

    // 设置指定位置的颜色，常量版本
    bool set(int x, int y, const TGAColor &c);

    // 析构函数，释放图像数据
    ~TGAImage();

    // 赋值运算符重载
    TGAImage &operator=(const TGAImage &img);

    // 获取图像宽度
    int get_width();

    // 获取图像高度
    int get_height();

    // 获取每像素字节数
    int get_bytespp();

    // 获取图像数据缓冲区
    unsigned char *buffer();

    // 清空图像数据
    void clear();
};

#endif  // __IMAGE_H__
