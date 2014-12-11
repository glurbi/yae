#include <math.h>

#include "matrix.hpp"

vector3f operator+(const vector3f& v1, const vector3f& v2)
{
    return vector3f(v1.v[0] + v2.v[0], v1.v[1] + v2.v[1], v1.v[2] + v2.v[2]);
}

vector3f operator-(const vector3f& v1, const vector3f& v2)
{
    return vector3f(v1.v[0] - v2.v[0], v1.v[1] - v2.v[1], v1.v[2] - v2.v[2]);
}

vector3f operator*(const vector3f& v, float t)
{
    return vector3f(t*v.v[0], t*v.v[1], t*v.v[2]);
}

vector3f operator*(const matrix44& m, const vector3f& v)
{
    vector4 v4(v, 1.0f);
    vector4 r;
    r.v[0] = m.m[0] * v4.v[0] + m.m[4] * v4.v[1] + m.m[8]  * v4.v[2] + m.m[12] * v4.v[3];
    r.v[1] = m.m[1] * v4.v[0] + m.m[5] * v4.v[1] + m.m[9]  * v4.v[2] + m.m[13] * v4.v[3];
    r.v[2] = m.m[2] * v4.v[0] + m.m[6] * v4.v[1] + m.m[10] * v4.v[2] + m.m[14] * v4.v[3];
    r.v[3] = m.m[3] * v4.v[0] + m.m[7] * v4.v[1] + m.m[11] * v4.v[2] + m.m[15] * v4.v[3];
    return vector3f(r.v);
}

vector4::vector4(float x, float y, float z, float w)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}

vector4::vector4()
{
}

vector4::vector4(const vector3f vec3, float w)
{
    v[0] = vec3.v[0];
    v[1] = vec3.v[1];
    v[2] = vec3.v[2];
    v[3] = w;
}

vector4::vector4(const vector3f vec3)
    : vector4(vec3, 1.0f)
{
}

color::color() : vector4(1, 1, 1, 1) {};
color::color(float r, float g, float b, float a) : vector4(r, g, b, a) {};
float color::r() const { return v[0]; }
float color::g() const { return v[1]; }
float color::b() const { return v[2]; }
float color::a() const { return v[3]; }

vector3f normalize(vector3f v) {
    float norm = sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
    return vector3f(v.v[0] / norm, v.v[1] / norm, v.v[2] / norm);
}

vector3f cross_product(const vector3f& u, const vector3f& v) {
    return vector3f(u.y()*v.z() - u.z()*v.y(), u.z()*v.x() - u.x()*v.z(), u.x()*v.y() - u.y()*v.x());
}

matrix44 multm(matrix44 m1, matrix44 m2) {
    matrix44 m;
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

matrix44 identity() {
    matrix44 mat;
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

matrix44 frustum(float left, float right, float bottom, float top, float nearp, float farp) {
    matrix44 mat;
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

matrix44 ortho(float left, float right, float bottom, float top, float nearp, float farp) {
    matrix44 mat;
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

matrix44 translation(float x, float y, float z) {
    matrix44 mat;
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

float to_radians(float a) {
    return a * 3.1415926f / 180.0f;
}

matrix44 rotation(float a, float x, float y, float z) {
    matrix44 mat;
    float c = (float)cos(to_radians(a));
    float s = (float)sin(to_radians(a));
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
matrix44 look_at(float eye_x, float eye_y, float eye_z, float center_x, float center_y, float center_z, float up_x, float up_y, float up_z) {
    auto f = normalize(vector3f(center_x - eye_x, center_y - eye_y, center_z - eye_z));
    auto up = normalize(vector3f(up_x, up_y, up_z));
    auto s = cross_product(f, up);
    auto u = cross_product(s, f);
    matrix44 m44;
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
