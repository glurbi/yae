#ifndef _geometry_hpp_
#define _geometry_hpp_

#include <vector>
#include <stack>
#include <memory>

#include <GL/glew.h>

namespace yae {

enum vertex_attribute {
    POSITION,
    TEXCOORD,
    NORMAL
};

template<class T, GLenum T_primitive = GL_QUADS>
struct geometry {

	geometry(GLsizei count, GLint dimensions)
        : count(count), positions_id(0), tex_coords_id(0), normals_id(0), dimensions(dimensions)
    {}

    ~geometry()
    {
        glDeleteBuffers(1, &positions_id);
        glDeleteBuffers(1, &tex_coords_id);
        glDeleteBuffers(1, &normals_id);
    }
    inline void set_vertex_positions(GLuint positionsId_)
    {
        positions_id = positionsId_;
    }

    inline void set_vertex_tex_coords(GLuint texCoordsId_)
    {
        tex_coords_id = texCoordsId_;
    }

    inline void set_vertex_normals(GLuint normalsId_)
    {
        normals_id = normalsId_;
    }

    void set_vertex_positions(void* data, long size)
    {
        glGenBuffers(1, &positions_id);
        glBindBuffer(GL_ARRAY_BUFFER, positions_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }
	void set_vertex_tex_coords(void* data, long size)
    {
        glGenBuffers(1, &tex_coords_id);
        glBindBuffer(GL_ARRAY_BUFFER, tex_coords_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void set_vertex_normals(void* data, long size)
    {
        glGenBuffers(1, &normals_id);
        glBindBuffer(GL_ARRAY_BUFFER, normals_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    inline GLuint get_positions_id() const
    {
        return positions_id;
    }

    inline GLuint get_tex_coords_id() const
    {
        return tex_coords_id;
    }

    inline GLuint get_normals_id() const
    {
        return normals_id;
    }

    inline GLsizei get_count() const
    {
        return count;
    }

    inline GLint get_dimensions() const
    {
        return dimensions;
    }

private:
	GLuint positions_id;
	GLuint tex_coords_id;
	GLuint normals_id;
    GLsizei count;
    GLint dimensions;
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

    geometry_builder(GLint dimensions)
        : dimensions(dimensions)
    {
        data.push(std::vector<T>());
    }

    std::unique_ptr<geometry<T>> build()
    {
        auto b = buffer_object_builder<T> { data.top() };
        auto g = std::make_unique<geometry<T>>(data.top().size() / dimensions, dimensions);
        g->set_vertex_positions(b.build());
        return g;
    }

    geometry_builder<T>& append(const std::vector<T>& v)
    {
        data.top().insert(data.top().end(), v.begin(), v.end());
        return *this;
    }

    geometry_builder<T>& make_grid(int nx, int ny);

    geometry_builder<T>& transform(const matrix44f& tr)
    {
        for (typename std::vector<T>::size_type i = 0; i < data.top().size(); i += 3) {
            vector3f v3 = tr * vector3f(&data.top()[i]);
            v3.copy(&data.top()[i]);
        }
        return *this;
    }

    geometry_builder<T>& begin()
    {
        data.push(std::vector<T>());
        return *this;
    }

    geometry_builder<T>& end()
    {
        std::vector<T> oldtop = std::move(data.top());
        data.pop();
        data.top().insert(data.top().end(), oldtop.begin(), oldtop.end());
        return *this;
    }

private:
    std::stack<std::vector<T>> data;
    GLint dimensions;
};

template<class T> std::unique_ptr<geometry<T>> make_grid(int nx, int ny);
template<class T> std::unique_ptr<geometry<T>> make_box(int nx, int ny, int nz);
template<class T> std::unique_ptr<geometry<T>> make_uv_sphere(int nlong, int nlat);

template <class T>
geometry_builder<T>& geometry_builder<T>::make_grid(int nx, int ny)
{
    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            auto v = std::vector<T> {
                (T)x,       (T)y,       (T)0,
                (T)(x + 1), (T)y,       (T)0,
                (T)(x + 1), (T)(y + 1), (T)0,
                (T)x,       (T)(y + 1), (T)0
            };
            data.top().insert(data.top().end(), v.begin(), v.end());
        }
    }
    return *this;
}

template <class T>
std::unique_ptr<geometry<T>> make_grid(int nx, int ny)
{
    auto geomb = geometry_builder<T>{3};
    geomb.begin();
    geomb.begin().make_grid(nx, ny).end();
    geomb.transform(translation((T)-nx / (T)2, (T)-ny / (T)2, (T)0));
    geomb.end();
    return geomb.build();
}

template <class T>
std::unique_ptr<geometry<T>> make_box(int nx, int ny, int nz)
{
    auto geomb = geometry_builder<T>{3};
    geomb.begin();
    geomb.begin().make_grid(nx, ny).end();
    geomb.begin().make_grid(nz, ny).transform(rotation(90.0f, 0.0f, 1.0f, 0.0f)).end();
    geomb.begin().make_grid(nz, ny).transform(rotation(90.0f, 0.0f, 1.0f, 0.0f)).transform(translation((T)nx, 0.0f, 0.0f)).end();
    geomb.begin().make_grid(nx, ny).transform(translation(0.0f, 0.0f, (T)-nz)).end();
    geomb.begin().make_grid(nx, nz).transform(rotation(-90.0f, 1.0f, 0.0f, 0.0f)).end();
    geomb.begin().make_grid(nx, nz).transform(rotation(-90.0f, 1.0f, 0.0f, 0.0f)).transform(translation(0.0f, (T)ny, 0.0f)).end();
    geomb.transform(translation((T)-nx / (T)2, (T)-ny / (T)2, (T)nz / (T)2));
    geomb.end();
    return geomb.build();
}

template<class T>
std::unique_ptr<geometry<T>> make_disco_sphere(int nlong, int nlat)
{
    auto geomb = geometry_builder<T>{3};
    return geomb.build();
}

}

#endif
