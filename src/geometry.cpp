#include "geometry.h"

// 专门化的构造函数，用于从浮点型向量转换为整数型向量
template <>
template <>
vec<3, int>::vec(const vec<3, float> &v)
    : x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)) {}

// 专门化的构造函数，用于从整数型向量转换为浮点型向量
template <>
template <>
vec<3, float>::vec(const vec<3, int> &v) : x(v.x), y(v.y), z(v.z) {}

// 专门化的构造函数，用于从浮点型二维向量转换为整数型二维向量
template <>
template <>
vec<2, int>::vec(const vec<2, float> &v)
    : x(int(v.x + .5f)), y(int(v.y + .5f)) {}

// 专门化的构造函数，用于从整数型二维向量转换为浮点型二维向量
template <>
template <>
vec<2, float>::vec(const vec<2, int> &v) : x(v.x), y(v.y) {}
