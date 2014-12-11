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
std::ostream& operator<<(std::ostream& os, const vector3<T>& v);

template<class T>
std::ostream& operator<<(std::ostream& os, const vector3<T>& v)
{
    os << '[' << v.x() << ' ' << v.y() << ' ' << v.z() << ']';
    return os;
}

typedef vector3<float> vector3f;

struct matrix44
{
    float m[16];
};

vector3f operator+(const vector3f& v1, const vector3f& v2);

vector3f operator-(const vector3f& v1, const vector3f& v2);

vector3f operator*(const vector3f& v, float t);

vector3f operator*(const matrix44& m, const vector3f& v);

struct vector4 {
    vector4(float x, float y, float z, float w);
    vector4(const vector3f vec3, float w);
    vector4(const vector3f vec3);
    vector4();
    float v[4];
};

struct color : vector4 {
    color();
    color(float r, float g, float b, float a = 1.0f);
    float r() const;
    float g() const;
    float b() const;
    float a() const;
};

vector3f normalize(vector3f v);

vector3f cross_product(const vector3f& u, const vector3f& v);

matrix44 multm(matrix44 m1, matrix44 m2);

template <class... M>
matrix44 multm(matrix44& m1, matrix44& m2, M&... m)
{
    return multm(m1, multm(m2, m...));
};

matrix44 identity();

matrix44 frustum(float left, float right, float bottom, float top, float nearp, float farp);

matrix44 ortho(float left, float right, float bottom, float top, float nearp, float farp);

matrix44 translation(float x, float y, float z);

float to_radians(float a);

matrix44 rotation(float a, float x, float y, float z);

matrix44 look_at(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

#endif
