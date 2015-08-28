#ifndef _matrix_hpp_
#define _matrix_hpp_

#include <iostream>
#include <cstring>

namespace yae {
    
template<class T>
struct vector3 {

    inline vector3() { v[0] = (T)0; v[1] = (T)0; v[2] = (T)0; }

    inline vector3(T x, T y, T z) { v[0] = x; v[1] = y; v[2] = z; }

    inline vector3(T* tp) { v[0] = tp[0]; v[1] = tp[1]; v[2] = tp[2]; }

    inline vector3(const vector3<T>& vec3) { vec3.copy(v); }

    inline T x() const { return v[0]; }
    inline T y() const { return v[1]; }
    inline T z() const { return v[2]; }

    inline void copy(T* dest) const { memcpy(dest, v, 3 * sizeof(T)); }

private:
    T v[3];
};

template<class T>
struct vector4 {

    inline vector4(T x, T y, T z, T w) { v[0] = x; v[1] = y; v[2] = z; v[3] = w; }

    inline vector4(const vector3<T>& vec3, T w) { v[0] = vec3.x(); v[1] = vec3.y(); v[2] = vec3.z(); v[3] = w; }

    inline vector4(const vector3<T>& vec3) : vector4(vec3, (T)1) {}

    inline T x() const { return v[0]; }
    inline T y() const { return v[1]; }
    inline T z() const { return v[2]; }
    inline T w() const { return v[3]; }

    inline void copy(T* dest) const { memcpy(dest, v, 4 * sizeof(T)); }

private:
    T v[4];
};

template<class T>
struct triangle {

    inline triangle(vector3<T> v1, vector3<T> v2, vector3<T> v3)
    {
        v1.copy(&t[0]);
        v2.copy(&t[3]);
        v3.copy(&t[6]);
    }

    inline vector3<T> v1() { return vector3<T>(&t[0]); }
    inline vector3<T> v2() { return vector3<T>(&t[3]); }
    inline vector3<T> v3() { return vector3<T>(&t[6]); }

    inline void copy(T* dest) const { memcpy(dest, &t[0], 9*sizeof(T)); }
    inline void append_to(std::vector<T>& vec) const { vec.insert(vec.end(), t, t + 9); }

private:
    T t[9];
};

template<class T>
struct matrix44 {
    T m[16];
};

template<class T>
struct color3 : vector3<T> {

    inline color3() : vector3<T>(1, 1, 1) {}

    inline color3(T r, T g, T b) : vector3<T>(r, g, b) {}

    inline T r() const { return x(); }
    inline T g() const { return y(); }
    inline T b() const { return z(); }
};

template<class T>
struct color4 : vector4<T> {

    inline color4() : vector4<T>(1, 1, 1, 1) {}

    inline color4(T r, T g, T b, T a = 1) : vector4<T>(r, g, b, a) {}

    inline T r() const { return x(); }
    inline T g() const { return y(); }
    inline T b() const { return z(); }
    inline T a() const { return w(); }
};

template<class T>
inline vector3<T> midpoint(const vector3<T>& v1, const vector3<T>& v2)
{
    return (v1 + v2) / (static_cast<T>(2));
}

template<class T>
std::ostream& operator<<(std::ostream& os, const vector3<T>& v)
{
    os << '[' << v.x() << ' ' << v.y() << ' ' << v.z() << ']';
    return os;
}

template<class T>
inline vector3<T> operator+(const vector3<T>& v1, const vector3<T>& v2)
{
    return vector3<T>(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z());
}

template<class T>
inline vector3<T> operator-(const vector3<T>& v1, const vector3<T>& v2)
{
    return vector3<T>(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
}

template<class T>
inline vector3<T> operator*(const vector3<T>& v, T t)
{
    return vector3<T>(t*v.x(), t*v.y(), t*v.z());
}

template<class T>
inline vector3<T> operator/(const vector3<T>& v, T t)
{
    return vector3<T>(v.x()/t, v.y()/t, v.z()/t);
}


template<class T>
inline vector3<T> operator*(const matrix44<T>& m, const vector3<T>& v)
{
    vector4<T> v4(v, (T)1);
    T x = m.m[0] * v4.x() + m.m[4] * v4.y() + m.m[8] * v4.z() + m.m[12] * v4.w();
    T y = m.m[1] * v4.x() + m.m[5] * v4.y() + m.m[9] * v4.z() + m.m[13] * v4.w();
    T z = m.m[2] * v4.x() + m.m[6] * v4.y() + m.m[10] * v4.z() + m.m[14] * v4.w();
    return vector3<T>(x, y, z);
}

template<class T>
inline vector3<T> normalize(const vector3<T>& v)
{
    T norm = sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
    return vector3<T>(v.x() / norm, v.y() / norm, v.z() / norm);
}

template<class T>
inline vector3<T> cross_product(const vector3<T>& u, const vector3<T>& v)
{
    return vector3<T>(u.y()*v.z() - u.z()*v.y(), u.z()*v.x() - u.x()*v.z(), u.x()*v.y() - u.y()*v.x());
}

template<class T>
matrix44<T> multm(matrix44<T> m1, matrix44<T> m2)
{
    matrix44<T> m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i + j * 4] =
                m1.m[i + 0] * m2.m[j * 4 + 0] +
                m1.m[i + 4] * m2.m[j * 4 + 1] +
                m1.m[i + 8] * m2.m[j * 4 + 2] +
                m1.m[i + 12] * m2.m[j * 4 + 3];
        }
    }
    return m;
}

template <class T, class... M>
matrix44<T> multm(matrix44<T> m1, matrix44<T> m2, M... m)
{
    return multm(m1, multm(m2, m...));
}

template<class T>
matrix44<T> identity()
{
    matrix44<T> mat;
    mat.m[0] = 1;
    mat.m[1] = 0;
    mat.m[2] = 0;
    mat.m[3] = 0;
    mat.m[4] = 0;
    mat.m[5] = 1;
    mat.m[6] = 0;
    mat.m[7] = 0;
    mat.m[8] = 0;
    mat.m[9] = 0;
    mat.m[10] = 1;
    mat.m[11] = 0;
    mat.m[12] = 0;
    mat.m[13] = 0;
    mat.m[14] = 0;
    mat.m[15] = 1;
    return mat;
}

template<class T>
matrix44<T> frustum(T left, T right, T bottom, T top, T nearp, T farp)
{
    matrix44<T> mat;
    mat.m[0] = 2 * nearp / (right - left);
    mat.m[1] = 0.0f;
    mat.m[2] = 0.0f;
    mat.m[3] = 0.0f;
    mat.m[4] = 0.0f;
    mat.m[5] = 2 * nearp / (top - bottom);
    mat.m[6] = 0.0f;
    mat.m[7] = 0.0f;
    mat.m[8] = (right + left) / (right - left);
    mat.m[9] = (top + bottom) / (top - bottom);
    mat.m[10] = -(farp + nearp) / (farp - nearp);
    mat.m[11] = -1.0f;
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = -2.0f * farp * nearp / (farp - nearp);
    mat.m[15] = 0.0f;
    return mat;
}

template<class T>
matrix44<T> ortho(T left, T right, T bottom, T top, T nearp, T farp)
{
    matrix44<T> mat;
    mat.m[0] = 2 / (right - left);
    mat.m[1] = 0.0f;
    mat.m[2] = 0.0f;
    mat.m[3] = 0.0f;
    mat.m[4] = 0.0f;
    mat.m[5] = 2 / (top - bottom);
    mat.m[6] = 0.0f;
    mat.m[7] = 0.0f;
    mat.m[8] = 0.0f;
    mat.m[9] = 0.0f;
    mat.m[10] = 2 / (farp - nearp);
    mat.m[11] = 0.0f;
    mat.m[12] = -(right + left) / (right - left);
    mat.m[13] = -(top + bottom) / (top - bottom);
    mat.m[14] = -(farp + nearp) / (farp - nearp);
    mat.m[15] = 1.0f;
    return mat;
}

template<class T>
matrix44<T> translation(T x, T y, T z)
{
    matrix44<T> mat;
    mat.m[0] = 1.0f;
    mat.m[1] = 0.0f;
    mat.m[2] = 0.0f;
    mat.m[3] = 0.0f;
    mat.m[4] = 0.0f;
    mat.m[5] = 1.0f;
    mat.m[6] = 0.0f;
    mat.m[7] = 0.0f;
    mat.m[8] = 0.0f;
    mat.m[9] = 0.0f;
    mat.m[10] = 1.0f;
    mat.m[11] = 0.0f;
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = z;
    mat.m[15] = 1.0f;
    return mat;
}

template<class T>
inline T to_radians(T deg)
{
    return deg * 3.1415926f / 180.0f;
}

template<class T>
matrix44<T> rotation(T deg, T x, T y, T z)
{
    matrix44<T> mat;
    T c = (T)cos(to_radians(deg));
    T s = (T)sin(to_radians(deg));
    mat.m[0] = x * x * (1 - c) + c;
    mat.m[1] = y * x * (1 - c) + z * s;
    mat.m[2] = x * z * (1 - c) - y * s;
    mat.m[3] = 0.0f;
    mat.m[4] = y * x * (1 - c) - z * s;
    mat.m[5] = y * y * (1 - c) + c;
    mat.m[6] = y * z * (1 - c) + x * s;
    mat.m[7] = 0.0f;
    mat.m[8] = x * z * (1 - c) + y * s;
    mat.m[9] = y * z * (1 - c) - x * s;
    mat.m[10] = z * z * (1 - c) + c;
    mat.m[11] = 0.0f;
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
    return mat;
}

template<class T>
matrix44<T> look_at(T eye_x, T eye_y, T eye_z, T center_x, T center_y, T center_z, T up_x, T up_y, T up_z)
{
    // cf gluLookAt http://www.unix.com/man-page/All/3/gluLookAt/
    auto f = normalize(vector3<T>(center_x - eye_x, center_y - eye_y, center_z - eye_z));
    auto up = normalize(vector3<T>(up_x, up_y, up_z));
    auto s = cross_product(f, up);
    auto u = cross_product(s, f);
    matrix44f m44;
    m44.m[0] = s.x();
    m44.m[1] = u.x();
    m44.m[2] = -f.x();
    m44.m[3] = 0;
    m44.m[4] = s.y();
    m44.m[5] = u.y();
    m44.m[6] = -f.y();
    m44.m[7] = 0;
    m44.m[8] = s.z();
    m44.m[9] = u.z();
    m44.m[10] = -f.z();
    m44.m[11] = 0;
    m44.m[12] = 0;
    m44.m[13] = 0;
    m44.m[14] = 0;
    m44.m[15] = 1;
    return multm(m44, translation(-eye_x, -eye_y, -eye_z));
}

typedef vector3<float> vector3f;
typedef matrix44<float> matrix44f;
typedef vector4<float> vector4f;
typedef color3<float> color3f;
typedef color4<float> color4f;

}

#endif

