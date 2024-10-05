#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const int width  = 200;
const int height = 200;

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }

    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

// mothod1: draw the triangle by drawing the three edges
// void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
//     line(t0, t1, image, color);
//     line(t1, t2, image, color);
//     line(t2, t0, image, color);
// }

// mothod2: more efficient, plus fill the triangle
// void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
//     if (t0.y == t1.y && t0.y == t2.y)
//         return;
//     // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
//     if (t0.y>t1.y) std::swap(t0, t1); 
//     if (t0.y>t2.y) std::swap(t0, t2); 
//     if (t1.y>t2.y) std::swap(t1, t2); 
//     int total_height = t2.y-t0.y; 
//     for (int y=t0.y; y<=t1.y; y++) { 
//         int segment_height = t1.y-t0.y+1; 
//         float alpha = (float)(y-t0.y)/total_height; 
//         float beta  = (float)(y-t0.y)/segment_height; // be careful with divisions by zero 
//         Vec2i A = t0 + (t2-t0)*alpha; 
//         Vec2i B = t0 + (t1-t0)*beta; 
//         if (A.x>B.x) std::swap(A, B); 
//         for (int j=A.x; j<=B.x; j++) { 
//             image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
//         } 
//     } 
//     for (int y=t1.y; y<=t2.y; y++) { 
//         int segment_height =  t2.y-t1.y+1; 
//         float alpha = (float)(y-t0.y)/total_height; 
//         float beta  = (float)(y-t1.y)/segment_height; // be careful with divisions by zero 
//         Vec2i A = t0 + (t2-t0)*alpha; 
//         Vec2i B = t1 + (t2-t1)*beta; 
//         if (A.x>B.x) std::swap(A, B); 
//         for (int j=A.x; j<=B.x; j++) { 
//             image.set(j, y, color); // attention, due to int casts t0.y+i != A.y 
//         } 
//     } 
// }

// mothod3: refactoring mothod2
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

int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

