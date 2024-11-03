#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

// mat 类的前向声明，用于后续的 dt 结构体
template <size_t DimCols, size_t DimRows, typename T>
class mat;

// 通用向量模板结构体，维度为 DIM
template <size_t DIM, typename T>
struct vec {
    // 默认构造函数，将所有元素初始化为零
    vec() { for (size_t i = DIM; i--; data_[i] = T()); }

    // 重载下标运算符，用于访问或修改元素
    T& operator[](const size_t i) {
        assert(i < DIM);  // 确保索引在有效范围内
        return data_[i];
    }
    const T& operator[](const size_t i) const {
        assert(i < DIM);
        return data_[i];
    }

private:
    T data_[DIM];  // 存储向量元素的数组
};

/////////////////////////////////////////////////////////////////////////////////

// 特化二维向量模板
template <typename T>
struct vec<2, T> {
    vec() : x(T()), y(T()) {}      // 默认构造函数，将 x 和 y 初始化为零
    vec(T X, T Y) : x(X), y(Y) {}  // 构造函数，使用给定值初始化 x 和 y
    template <class U>
    vec<2, T>(const vec<2, U>& v);  // 复制构造函数

    // 重载下标运算符
    T& operator[](const size_t i) {
        assert(i < 2);  // 确保索引在有效范围内
        return i <= 0 ? x : y;
    }
    const T& operator[](const size_t i) const {
        assert(i < 2);
        return i <= 0 ? x : y;
    }

    T x, y;  // 向量的 x 和 y 分量
};

/////////////////////////////////////////////////////////////////////////////////

// 特化三维向量模板
template <typename T>
struct vec<3, T> {
    vec() : x(T()), y(T()), z(T()) {}         // 默认构造函数，初始化为零
    vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}  // 构造函数，使用给定值初始化
    template <class U>
    vec<3, T>(const vec<3, U>& v);  // 复制构造函数

    // 重载下标运算符
    T& operator[](const size_t i) {
        assert(i < 3);  // 确保索引在有效范围内
        return i <= 0 ? x : (1 == i ? y : z);
    }
    const T& operator[](const size_t i) const {
        assert(i < 3);
        return i <= 0 ? x : (1 == i ? y : z);
    }

    // 计算向量的模长
    float norm() { return std::sqrt(x * x + y * y + z * z); }

    // 将向量归一化
    vec<3, T>& normalize(T l = 1) {
        *this = (*this) * (l / norm());
        return *this;
    }

    T x, y, z;  // 向量的 x、y 和 z 分量
};

/////////////////////////////////////////////////////////////////////////////////

// 向量的点积
template <size_t DIM, typename T>
T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    T ret = T();
    for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);
    return ret;
}

// 向量的加法运算
template <size_t DIM, typename T>
vec<DIM, T> operator+(vec<DIM, T> lhs, const vec<DIM, T>& rhs) {
    for (size_t i = DIM; i--; lhs[i] += rhs[i]);
    return lhs;
}

// 向量的减法运算
template <size_t DIM, typename T>
vec<DIM, T> operator-(vec<DIM, T> lhs, const vec<DIM, T>& rhs) {
    for (size_t i = DIM; i--; lhs[i] -= rhs[i]);
    return lhs;
}

// 向量与标量的乘法运算
template <size_t DIM, typename T, typename U>
vec<DIM, T> operator*(vec<DIM, T> lhs, const U& rhs) {
    for (size_t i = DIM; i--; lhs[i] *= rhs);
    return lhs;
}

// 向量与标量的除法运算
template <size_t DIM, typename T, typename U>
vec<DIM, T> operator/(vec<DIM, T> lhs, const U& rhs) {
    for (size_t i = DIM; i--; lhs[i] /= rhs);
    return lhs;
}

// 嵌入向量，将较小维度的向量嵌入到更高维度中
template <size_t LEN, size_t DIM, typename T>
vec<LEN, T> embed(const vec<DIM, T>& v, T fill = 1) {
    vec<LEN, T> ret;
    for (size_t i = LEN; i--; ret[i] = (i < DIM ? v[i] : fill));
    return ret;
}

// 投影向量，将高维向量投影到较低维度
template <size_t LEN, size_t DIM, typename T>
vec<LEN, T> proj(const vec<DIM, T>& v) {
    vec<LEN, T> ret;
    for (size_t i = LEN; i--; ret[i] = v[i]);
    return ret;
}

// 三维向量的叉积
template <typename T>
vec<3, T> cross(vec<3, T> v1, vec<3, T> v2) {
    return vec<3, T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
                     v1.x * v2.y - v1.y * v2.x);
}

// 向量的输出运算符
template <size_t DIM, typename T>
std::ostream& operator<<(std::ostream& out, vec<DIM, T>& v) {
    for (unsigned int i = 0; i < DIM; i++) {
        out << v[i] << " ";
    }
    return out;
}

/////////////////////////////////////////////////////////////////////////////////

// 计算矩阵的行列式
template <size_t DIM, typename T>
struct dt {
    static T det(const mat<DIM, DIM, T>& src) {
        T ret = 0;
        for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

// 一维矩阵的行列式特化
template <typename T>
struct dt<1, T> {
    static T det(const mat<1, 1, T>& src) { return src[0][0]; }
};

/////////////////////////////////////////////////////////////////////////////////

// 矩阵类模板
template <size_t DimRows, size_t DimCols, typename T>
class mat {
    vec<DimCols, T> rows[DimRows];  // 用二维向量存储矩阵的行

public:
    mat() {}

    // 重载下标运算符，用于访问行向量
    vec<DimCols, T>& operator[](const size_t idx) {
        assert(idx < DimRows);
        return rows[idx];
    }

    const vec<DimCols, T>& operator[](const size_t idx) const {
        assert(idx < DimRows);
        return rows[idx];
    }

    // 获取矩阵的指定列
    vec<DimRows, T> col(const size_t idx) const {
        assert(idx < DimCols);
        vec<DimRows, T> ret;
        for (size_t i = DimRows; i--; ret[i] = rows[i][idx]);
        return ret;
    }

    // 设置矩阵的指定列
    void set_col(size_t idx, vec<DimRows, T> v) {
        assert(idx < DimCols);
        for (size_t i = DimRows; i--; rows[i][idx] = v[i]);
    }

    // 返回单位矩阵
    static mat<DimRows, DimCols, T> identity() {
        mat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--;)
            for (size_t j = DimCols; j--; ret[i][j] = (i == j));
        return ret;
    }

    // 计算行列式
    T det() const { return dt<DimCols, T>::det(*this); }

    // 获取矩阵的余子式
    mat<DimRows - 1, DimCols - 1, T> get_minor(size_t row, size_t col) const {
        mat<DimRows - 1, DimCols - 1, T> ret;
        for (size_t i = DimRows - 1; i--;)
            for (size_t j = DimCols - 1; j--;
                 ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        return ret;
    }

    // 计算矩阵的代数余子式
    T cofactor(size_t row, size_t col) const {
        return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
    }

    // 计算伴随矩阵
    mat<DimRows, DimCols, T> adjugate() const {
        mat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--;)
            for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j));
        return ret;
    }

    // 计算逆矩阵的转置
    mat<DimRows, DimCols, T> invert_transpose() {
        mat<DimRows, DimCols, T> ret = adjugate();
        T tmp = ret[0] * rows[0];
        return ret / tmp;
    }

    // 计算逆矩阵
    mat<DimCols, DimRows, T> invert() { return invert_transpose().transpose(); }

    // 转置矩阵
    mat<DimCols, DimRows, T> transpose() {
        mat<DimCols, DimRows, T> ret;
        for (size_t i = DimRows; i--; ret[i] = this->col(i));
        return ret;
    }
};

/////////////////////////////////////////////////////////////////////////////////

// 矩阵和向量的乘法
template <size_t DimRows, size_t DimCols, typename T>
vec<DimRows, T> operator*(const mat<DimRows, DimCols, T>& lhs,
                          const vec<DimCols, T>& rhs) {
    vec<DimRows, T> ret;
    for (size_t i = DimRows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

// 矩阵乘法
template <size_t R1, size_t C1, size_t C2, typename T>
mat<R1, C2, T> operator*(const mat<R1, C1, T>& lhs, const mat<C1, C2, T>& rhs) {
    mat<R1, C2, T> result;
    for (size_t i = R1; i--;)
        for (size_t j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
    return result;
}

// 矩阵除法
template <size_t DimRows, size_t DimCols, typename T>
mat<DimCols, DimRows, T> operator/(mat<DimRows, DimCols, T> lhs, const T& rhs) {
    for (size_t i = DimRows; i--; lhs[i] = lhs[i] / rhs);
    return lhs;
}

// 矩阵输出运算符
template <size_t DimRows, size_t DimCols, class T>
std::ostream& operator<<(std::ostream& out, mat<DimRows, DimCols, T>& m) {
    for (size_t i = 0; i < DimRows; i++) out << m[i] << std::endl;
    return out;
}

/////////////////////////////////////////////////////////////////////////////////

// 常用的向量和矩阵类型别名
typedef vec<2, float> Vec2f;
typedef vec<2, int> Vec2i;
typedef vec<3, float> Vec3f;
typedef vec<3, int> Vec3i;
typedef vec<4, float> Vec4f;
typedef mat<4, 4, float> Matrix;

#endif  // __GEOMETRY_H__
