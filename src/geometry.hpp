#ifndef _geometry_hpp_
#define _geometry_hpp_

#include <vector>
#include <stack>
#include <memory>

#include <GL/glew.h>

namespace yae {

enum vertex_attribute : GLuint {
    POSITION,
    TEXCOORD,
    NORMAL
};

template<class T>
struct geometry {

	geometry(GLsizei count, GLint dimensions, GLenum primitive_type)
    : count(count), _positions_id(0), _tex_coords_id(0),
      _normals_id(0), dimensions(dimensions), primitive_type(primitive_type)
    {}

    ~geometry()
    {
        glDeleteBuffers(1, &_positions_id);
        glDeleteBuffers(1, &_tex_coords_id);
        glDeleteBuffers(1, &_normals_id);
    }

    inline void set_vertex_positions(GLuint positions_id)
    {
        _positions_id = positions_id;
    }

    inline void set_vertex_tex_coords(GLuint tex_coords_id)
    {
        _tex_coords_id = tex_coords_id;
    }

    inline void set_vertex_normals(GLuint normals_id)
    {
        _normals_id = normals_id;
    }

    void set_vertex_positions(void* data, long size)
    {
        glGenBuffers(1, &_positions_id);
        glBindBuffer(GL_ARRAY_BUFFER, _positions_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

	void set_vertex_tex_coords(void* data, long size)
    {
        glGenBuffers(1, &_tex_coords_id);
        glBindBuffer(GL_ARRAY_BUFFER, _tex_coords_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void set_vertex_normals(void* data, long size)
    {
        glGenBuffers(1, &_normals_id);
        glBindBuffer(GL_ARRAY_BUFFER, _normals_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    inline GLuint get_positions_id() const
    {
        return _positions_id;
    }

    inline GLuint get_tex_coords_id() const
    {
        return _tex_coords_id;
    }

    inline GLuint get_normals_id() const
    {
        return _normals_id;
    }

    inline GLsizei get_count() const
    {
        return count;
    }

    inline GLint get_dimensions() const
    {
        return dimensions;
    }

    inline GLuint get_primitive_type() const
    {
        return primitive_type;
    }

private:
	GLuint _positions_id;
	GLuint _tex_coords_id;
	GLuint _normals_id;
    GLsizei count;
    GLint dimensions;
    GLuint primitive_type;
};

template<class T>
struct buffer_object_builder {

    inline buffer_object_builder(std::vector<T> data)
        : _data(data)
    {}

    inline void* get_data()
    {
        return &_data[0];
    }

    inline GLsizeiptr get_size()
    {
        return _data.size() * sizeof(T);
    }

    inline GLsizeiptr get_count()
    {
        return _data.size();
    }

    GLuint build()
    {
        GLuint id;
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(T), &_data[0], GL_STATIC_DRAW);
        return id;
    }

private:
    std::vector<T> _data;
};

template<class T>
struct geometry_builder {

    geometry_builder(GLint dim, GLenum primitive_type)
    : _dim(dim), primitive_type(primitive_type)
    {
        _data.push(std::vector<T>());
    }

    std::unique_ptr<geometry<T>> build()
    {
        auto b = buffer_object_builder<T> { _data.top() };
        auto g = std::make_unique<geometry<T>>(_data.top().size() / _dim, _dim, primitive_type);
        g->set_vertex_positions(b.build());
        return g;
    }

    std::vector<T> data() const
    {
        return std::vector<T>(_data.top());
    }

    geometry_builder<T>& append(const vector3<T>& v)
    {
        _data.top().push_back(v.x());
        _data.top().push_back(v.y());
        _data.top().push_back(v.z());
        return *this;
    }

    geometry_builder<T>& append(const triangle<T>& tr)
    {
        tr.append_to(_data.top());
        return *this;
    }

    geometry_builder<T>& append(const std::vector<T>& v)
    {
        _data.top().insert(_data.top().end(), v.begin(), v.end());
        return *this;
    }

    geometry_builder<T>& transform(const matrix44f& tr)
    {
        for (typename std::vector<T>::size_type i = 0; i < _data.top().size(); i += _dim) {
            vector3f v3 = tr * vector3f(&_data.top()[i]);
            v3.copy(&_data.top()[i]);
        }
        return *this;
    }

    geometry_builder<T>& begin()
    {
        _data.push(std::vector<T>());
        return *this;
    }

    geometry_builder<T>& end()
    {
        std::vector<T> oldtop = std::move(_data.top());
        _data.pop();
        _data.top().insert(_data.top().end(), oldtop.begin(), oldtop.end());
        return *this;
    }

private:
    std::stack<std::vector<T>> _data;
    GLint _dim;
    GLenum primitive_type;
};

template <class T>
std::vector<T> make_grid_data(int nx, int ny)
{
    std::vector<T> data;
    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            auto v = std::vector<T> {
                (T)x, (T)y, (T)0,
                    (T)(x + 1), (T)y, (T)0,
                    (T)(x + 1), (T)(y + 1), (T)0,
                    (T)x, (T)(y + 1), (T)0
            };
            data.insert(data.end(), v.begin(), v.end());
        }
    }
    return data;
}


template <class T>
geometry_builder<T> make_grid(int nx, int ny)
{
    auto geomb = geometry_builder<T>{3};
    geomb.begin();
    geomb.begin().append(make_grid_data<T>(nx, ny).data()).end();
    geomb.transform(translation((T)-nx / (T)2, (T)-ny / (T)2, (T)0));
    geomb.end();
    return geomb;
}

template <class T>
geometry_builder<T> make_box(int nx, int ny, int nz)
{
    auto geomb = geometry_builder<T>{3, GL_QUADS};
    geomb.begin();
    geomb.begin().append(make_grid_data<T>(nx, ny)).end();
    geomb.begin().append(make_grid_data<T>(nz, ny)).transform(rotation(90.0f, 0.0f, 1.0f, 0.0f)).end();
    geomb.begin().append(make_grid_data<T>(nz, ny)).transform(rotation(90.0f, 0.0f, 1.0f, 0.0f)).transform(translation((T)nx, 0.0f, 0.0f)).end();
    geomb.begin().append(make_grid_data<T>(nx, ny)).transform(translation(0.0f, 0.0f, (T)-nz)).end();
    geomb.begin().append(make_grid_data<T>(nx, nz)).transform(rotation(-90.0f, 1.0f, 0.0f, 0.0f)).end();
    geomb.begin().append(make_grid_data<T>(nx, nz)).transform(rotation(-90.0f, 1.0f, 0.0f, 0.0f)).transform(translation(0.0f, (T)ny, 0.0f)).end();
    geomb.transform(translation((T)-nx / (T)2, (T)-ny / (T)2, (T)nz / (T)2));
    geomb.end();
    return geomb;
}

template<class T>
geometry_builder<T> make_uv_sphere(int nlong, int nlat)
{
    auto s = [](T teta, T phi) {
        // cf http://en.wikipedia.org/wiki/Sphere
        T x = static_cast<T>(1.0 * sin(teta) * cos(phi));
        T y = static_cast<T>(1.0 * sin(teta) * sin(phi));
        T z = static_cast<T>(1.0 * cos(teta));
        return vector3<T>(x, y, z);
    };
    auto geomb = geometry_builder<T>{3, GL_QUADS};
    geomb.begin();
    const T pi = static_cast<T>(3.14159265359);
    T step_long = 2 * pi / nlong;
    T step_lat = pi / nlat;
    for (int i = 0; i < nlong; i++) {
        for (int j = 0; j < nlat; j++) {
            T teta = j * step_lat;
            T phi = i * step_long;
            geomb.append(s(teta, phi));
            geomb.append(s(teta + step_lat, phi));
            geomb.append(s(teta + step_lat, phi + step_long));
            geomb.append(s(teta, phi + step_long));
        }
    }
    geomb.end();
    return geomb;
}

template<class T>
geometry_builder<T> make_octahedron_sphere(int n)
{
    auto geomb = geometry_builder<T>{3, GL_TRIANGLES};
    std::function<void(int, triangle<T>&)> refine = [&](int depth, triangle<T>& tr)
    {
        if (depth == n)
            geomb.append(tr);
        else {
            vector3<T> m1 = normalize(midpoint(tr.v2(), tr.v3()));
            vector3<T> m2 = normalize(midpoint(tr.v3(), tr.v1()));
            vector3<T> m3 = normalize(midpoint(tr.v1(), tr.v2()));
            refine(depth + 1, triangle<T>(tr.v1(), m3, m2));
            refine(depth + 1, triangle<T>(m3, tr.v2(), m1));
            refine(depth + 1, triangle<T>(m1, m2, m3));
            refine(depth + 1, triangle<T>(m2, m1, tr.v3()));
        }
    };
    vector3<T> v1 = { (T)0, (T)1, (T)0 };
    vector3<T> v2 = { (T)0, (T)0, (T)1 };
    vector3<T> v3 = { (T)1, (T)0, (T)0 };
    vector3<T> v4 = { (T)0, (T)0, (T)-1 };
    vector3<T> v5 = { (T)-1, (T)0, (T)0 };
    vector3<T> v6 = { (T)0, (T)-1, (T)0 };
    refine(0, triangle<T>(v1, v2, v3));
    refine(0, triangle<T>(v1, v3, v4));
    refine(0, triangle<T>(v1, v4, v5));
    refine(0, triangle<T>(v1, v2, v5));
    refine(0, triangle<T>(v6, v3, v2));
    refine(0, triangle<T>(v6, v2, v5));
    refine(0, triangle<T>(v6, v5, v4));
    refine(0, triangle<T>(v6, v4, v3));
    return geomb;
}

}

#endif
