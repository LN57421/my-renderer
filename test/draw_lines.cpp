#include <iostream>
#include <chrono> // for time calculation
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

int main(int argc, char **argv)
{
    TGAImage image(500, 500, TGAImage::RGB);
    int iterations = 1000000;  // multiple try

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        line(13, 20, 80, 40, image, white);
        line(20, 13, 40, 80, image, red);
        line(80, 40, 13, 20, image, red);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Average execution time: " << (elapsed.count()) << " seconds" << std::endl;

    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}

// Bresenham's line algorithm, we consider the following cases:
// 1. if the slope is greater than 1 (steep), we transpose the image and swap x and y in the loop
// 2. if x0 > x1, we swap the points because we always draw from left to right
// 3. if y0 > y1, we negate the y step value because we always draw from bottom to top
// 4. for optimization, we avoid floating point arithmetic

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int error = 0;  // int error 
    int ystep = (y0 < y1) ? 1 : -1;  // y stap
    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }
        error += dy;
        if (2 * error >= dx)  // using int instead of float 
        {
            y += ystep;
            error -= dx;
        }
    }
}
