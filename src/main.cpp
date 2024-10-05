#include <vector>
#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model *model = NULL;
const int width = 800;
const int height = 800;

Vec3f light_dir(0, 0, -1);  // define light_dir

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++)
    {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y == t1.y && t0.y == t2.y)
        return;  // I dont care about degenerate triangles
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!)
    if (t0.y > t1.y)
        std::swap(t0, t1);
    if (t0.y > t2.y)
        std::swap(t0, t2);
    if (t1.y > t2.y)
        std::swap(t1, t2);
    int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; i++) {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;

        // linear interpolation factor
        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) /
                     segment_height;  // be careful: with above conditions no
                                      // division by zero here
        //
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;

        if (A.x > B.x)
            std::swap(A, B);
        for (int j = A.x; j <= B.x; j++) {
            image.set(j, t0.y + i,
                      color);  // attention, due to int casts t0.y+i != A.y
        }
    }
}

int main(int argc, char **argv)
{
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model(
            "obj\\african_head.obj");
        if (model->nfaces() == 0) {
            std::cout << "Model has no faces." << std::endl;
        }
    }

    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++)
    {
        // line model
        // std::vector<int> face = model->face(i);
        // for (int j = 0; j < 3; j++)
        // {
        //     Vec3f v0 = model->vert(face[j]);
        //     Vec3f v1 = model->vert(face[(j + 1) % 3]);

        //     // Viewport Transformation: (point + 1) * width(height) / 2
        //     // point => [-1, 1]; point + 1 => [0, 2];  point => [0, width(height)]
        //     int x0 = (v0.x + 1.) * width / 2.;
        //     int y0 = (v0.y + 1.) * height / 2.;
        //     int x1 = (v1.x + 1.) * width / 2.;
        //     int y1 = (v1.y + 1.) * height / 2.;

        //     line(x0, y0, x1, y1, image, white);
        // }

        // triangle model
        std::vector<int> face =
            model->face(i);  //
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] =
                Vec2i((v.x + 1.) * width / 2.,
                      (v.y + 1.) * height / 2.);  // screen
            world_coords[j] = v; // world
        }
        // 用世界坐标计算法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^
                  (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;  // 光照强度=法向量*光照方向
                                          // 即法向量和光照方向重合时，亮度最高
        // 强度小于0，说明平面朝向为内  即背面裁剪
        if (intensity > 0) {
            triangle(screen_coords[0], screen_coords[1], screen_coords[2],
                     image,
                     TGAColor(intensity * 255, intensity * 255, intensity * 255,
                              255));
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
