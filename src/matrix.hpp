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
matrix44<T> multm(matrix44<T> m1, matrix44<T> m2, M... m);

template <class T>
matrix44<T> identity();

template <class T>
matrix44<T> frustum(T left, T right, T bottom, T top, T nearp, T farp);

template <class T>
matrix44<T> ortho(T left, T right, T bottom, T top, T nearp, T farp);

template<class T>
matrix44<T> translation(T x, T y, T z);

template<class T>
T to_radians(T deg);

template<class T>
matrix44<T> rotation(T deg, T x, T y, T z);

template<class T>
matrix44<T> look_at(T eyeX, T eyeY, T eyeZ, T centerX, T centerY, T centerZ, T upX, T upY, T upZ);

typedef vector3<float> vector3f;
typedef matrix44<float> matrix44f;
typedef vector4<float> vector4f;
typedef color3<float> color3f;
typedef color4<float> color4f;

#include "matrix_impl.hpp"

#endif
