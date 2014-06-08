#ifndef _matrix_hpp_
#define _matrix_hpp_

struct vector3 {
    vector3();
    vector3(float x, float y, float z);
    float v[3];
    float x() const;
    float y() const;
    float z() const;
};

vector3 operator+(const vector3& v1, const vector3& v2);

vector3 operator-(const vector3& v1, const vector3& v2);

vector3 operator*(const vector3& v, float t);

struct vector4 {
    vector4(float x, float y, float z, float w);
    vector4(const vector3 vec3, float w);
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

vector3 normalize(vector3 v);

vector3 cross_product(const vector3& u, const vector3& v);

struct matrix44
{
    float m[16];
};

matrix44 multm(matrix44 m1, matrix44 m2);

matrix44 identity();

matrix44 frustum(float left, float right, float bottom, float top, float nearp, float farp);

matrix44 ortho(float left, float right, float bottom, float top, float nearp, float farp);

matrix44 translation(float x, float y, float z);

float to_radians(float a);

matrix44 rotation(float a, float x, float y, float z);

matrix44 look_at(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

#endif
