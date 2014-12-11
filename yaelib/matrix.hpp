#ifndef _matrix_hpp_
#define _matrix_hpp_

#include <iostream>

template<class T>
struct vector3 {
    vector3();
    vector3(T x, T y, T z);
    vector3(T* tp);
    T v[3];
    T x() const;
    T y() const;
    T z() const;
    void copy(T* dest);
};

template<class T>
struct vector4 {
    vector4(T x, T y, T z, T w);
    vector4(const vector3<T> vec3, T w);
    vector4(const vector3<T> vec3);
    vector4();
    T v[4];
    T x() const;
    T y() const;
    T z() const;
    T w() const;
    void copy(T* dest);
};

template<class T>
struct matrix44 {
    T m[16];
};

template<class T>
struct color3 : vector3<T> {
    color3();
    color3(T r, T g, T b);
    T r() const;
    T g() const;
    T b() const;
};

template<class T>
struct color4 : vector4<T> {
    color4();
    color4(T r, T g, T b, T a = (T)1);
    T r() const;
    T g() const;
    T b() const;
    T a() const;
};

template<class T>
std::ostream& operator<<(std::ostream& os, const vector3<T>& v);

template<class T>
vector3<T> operator+(const vector3<T>& v1, const vector3<T>& v2);

template<class T>
vector3<T> operator-(const vector3<T>& v1, const vector3<T>& v2);

template<class T>
vector3<T> operator*(const vector3<T>& v, T t);

template<class T>
vector3<T> operator*(const matrix44<T>& m, const vector3<T>& v);

template<class T>
vector3<T> normalize(vector3<T> v);

template<class T>
vector3<T> cross_product(const vector3<T>& u, const vector3<T>& v);

template<class T>
matrix44<T> multm(matrix44<T> m1, matrix44<T> m2);

template <class T, class... M>
matrix44<T> multm(matrix44<T>& m1, matrix44<T>& m2, M&... m)
{
    return multm(m1, multm(m2, m...));
};

template <class T>
matrix44<T> identity();

template <class T>
matrix44<T> frustum(T left, T right, T bottom, T top, T nearp, T farp);

template <class T>
matrix44<T> ortho(T left, T right, T bottom, T top, T nearp, T farp);

template<class T>
matrix44<T> translation(T x, T y, T z);

template<class T>
T to_radians(T a);

template<class T>
matrix44<T> rotation(T a, T x, T y, T z);

template<class T>
matrix44<T> look_at(T eyeX, T eyeY, T eyeZ, T centerX, T centerY, T centerZ, T upX, T upY, T upZ);

typedef vector3<float> vector3f;
typedef matrix44<float> matrix44f;
typedef vector4<float> vector4f;
typedef color3<float> color3f;
typedef color4<float> color4f;

template<class T>
vector3<T>::vector3()
{
    v[0] = (T)0;
    v[1] = (T)0;
    v[2] = (T)0;
}

template<class T>
vector3<T>::vector3(T x, T y, T z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

template<class T>
vector3<T>::vector3(T* tp)
{
    v[0] = tp[0];
    v[1] = tp[1];
    v[2] = tp[2];
}

template<class T>
void vector3<T>::copy(T* dest)
{
    dest[0] = v[0];
    dest[1] = v[1];
    dest[2] = v[2];
}

template<class T>
T vector3<T>::x() const
{
    return v[0];
}

template<class T>
T vector3<T>::y() const
{
    return v[1];
}

template<class T>
T vector3<T>::z() const
{
    return v[2];
}

template<class T>
std::ostream& operator<<(std::ostream& os, const vector3<T>& v)
{
    os << '[' << v.x() << ' ' << v.y() << ' ' << v.z() << ']';
    return os;
}

template<class T>
vector3<T> operator+(const vector3<T>& v1, const vector3<T>& v2)
{
    return vector3<T>(v1.v[0] + v2.v[0], v1.v[1] + v2.v[1], v1.v[2] + v2.v[2]);
}

template<class T>
vector3<T> operator-(const vector3<T>& v1, const vector3<T>& v2)
{
    return vector3<T>(v1.v[0] - v2.v[0], v1.v[1] - v2.v[1], v1.v[2] - v2.v[2]);
}

template<class T>
vector3<T> operator*(const vector3<T>& v, T t)
{
    return vector3<T>(t*v.v[0], t*v.v[1], t*v.v[2]);
}

template<class T>
vector3<T> operator*(const matrix44<T>& m, const vector3<T>& v)
{
    vector4<T> v4(v, (T)1);
    vector4<T> r;
    r.v[0] = m.m[0] * v4.v[0] + m.m[4] * v4.v[1] + m.m[8] * v4.v[2] + m.m[12] * v4.v[3];
    r.v[1] = m.m[1] * v4.v[0] + m.m[5] * v4.v[1] + m.m[9] * v4.v[2] + m.m[13] * v4.v[3];
    r.v[2] = m.m[2] * v4.v[0] + m.m[6] * v4.v[1] + m.m[10] * v4.v[2] + m.m[14] * v4.v[3];
    r.v[3] = m.m[3] * v4.v[0] + m.m[7] * v4.v[1] + m.m[11] * v4.v[2] + m.m[15] * v4.v[3];
    return vector3<T>(r.v);
}

template<class T>
vector4<T>::vector4(T x, T y, T z, T w)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}

template<class T>
vector4<T>::vector4()
{
}

template<class T>
vector4<T>::vector4(const vector3<T> vec3, T w)
{
    v[0] = vec3.v[0];
    v[1] = vec3.v[1];
    v[2] = vec3.v[2];
    v[3] = w;
}

template<class T>
vector4<T>::vector4(const vector3<T> vec3)
    : vector4(vec3, (T)1)
{
}

template<class T>
color4<T>::color4()
    : vector4<T>(1, 1, 1, 1)
{
}

template<class T>
color4<T>::color4(T r, T g, T b, T a)
    : vector4<T>(r, g, b, a)
{
}

template<class T>
T color4<T>::r() const
{
    return this->v[0];
}

template<class T>
T color4<T>::g() const
{
    return this->v[1];
}

template<class T>
T color4<T>::b() const
{
    return this->v[2];
}

template<class T>
T color4<T>::a() const
{
    return this->v[3];
}

template<class T>
vector3<T> normalize(vector3<T> v) {
    T norm = sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
    return vector3<T>(v.v[0] / norm, v.v[1] / norm, v.v[2] / norm);
}

template<class T>
vector3<T> cross_product(const vector3<T>& u, const vector3<T>& v) {
    return vector3<T>(u.y()*v.z() - u.z()*v.y(), u.z()*v.x() - u.x()*v.z(), u.x()*v.y() - u.y()*v.x());
}

template<class T>
matrix44<T> multm(matrix44<T> m1, matrix44<T> m2) {
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

template<class T>
matrix44<T> identity() {
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
matrix44<T> frustum(T left, T right, T bottom, T top, T nearp, T farp) {
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
matrix44<T> ortho(T left, T right, T bottom, T top, T nearp, T farp) {
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
matrix44<T> translation(T x, T y, T z) {
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
T to_radians(T a) {
    return a * 3.1415926f / 180.0f;
}

template<class T>
matrix44<T> rotation(T a, T x, T y, T z) {
    matrix44<T> mat;
    T c = (T)cos(to_radians(a));
    T s = (T)sin(to_radians(a));
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

// cf gluLookAt http://www.unix.com/man-page/All/3/gluLookAt/
template<class T>
matrix44<T> look_at(T eye_x, T eye_y, T eye_z, T center_x, T center_y, T center_z, T up_x, T up_y, T up_z) {
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

#endif
