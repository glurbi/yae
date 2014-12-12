#ifndef _geometry_hpp_
#define _geometry_hpp_

#include <vector>
#include <memory>
#include <GL/glew.h>

enum vertex_attribute {
    POSITION,
    TEXCOORD,
    NORMAL
};

template<class T, GLenum T_primitive = GL_QUADS>
class geometry {

public:
	geometry(GLsizei count, GLint dimensions)
        : count(count), positions_id(0), tex_coords_id(0), normals_id(0), dimensions(dimensions) {}
	
	~geometry() {
        glDeleteBuffers(1, &positions_id);
        glDeleteBuffers(1, &tex_coords_id);
        glDeleteBuffers(1, &normals_id);
    }

	void set_vertex_positions(GLuint positionsId_) {
        positions_id = positionsId_;
    }

    void set_vertex_tex_coords(GLuint texCoordsId_) {
        tex_coords_id = texCoordsId_;
    }

    void set_vertex_normals(GLuint normalsId_) {
        normals_id = normalsId_;
    }

    void set_vertex_positions(void* data, long size) {
        glGenBuffers(1, &positions_id);
        glBindBuffer(GL_ARRAY_BUFFER, positions_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

	void set_vertex_tex_coords(void* data, long size) {
        glGenBuffers(1, &tex_coords_id);
        glBindBuffer(GL_ARRAY_BUFFER, tex_coords_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void set_vertex_normals(void* data, long size) {
        glGenBuffers(1, &normals_id);
        glBindBuffer(GL_ARRAY_BUFFER, normals_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    GLuint get_positions_id() const {
        return positions_id;
    }

    GLuint get_tex_coords_id() const {
        return tex_coords_id;
    }

    GLuint get_normals_id() const {
        return normals_id;
    }

    GLsizei get_count() const {
        return count;
    }

    GLint get_dimensions() const
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

template <class T>
class buffer_object_builder {

public:

    buffer_object_builder(std::vector<T> data_) {
        data = data_;
    }

    void* get_data() {
        return &data[0];
    }

    GLsizeiptr get_size() {
        return data.size() * sizeof(T);
    }

    GLsizeiptr get_count() {
        return data.size();
    }

    GLuint build() {
        GLuint id;
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
        return id;
    }

private:

    std::vector<T> data;

};

template <class T>
struct geometry_builder {
    geometry_builder(GLint dimensions)
        : dimensions(dimensions)
    {
    }
    std::unique_ptr<geometry<T>> build()
    {
        store();
        auto b = buffer_object_builder<T> { data };
        auto g = std::make_unique<geometry<T>>(data.size() / dimensions, dimensions);
        g->set_vertex_positions(b.build());
        return g;
    }
    geometry_builder<T>& operator<<(const std::vector<T>& v)
    {
        data.insert(data.end(), v.begin(), v.end());
        return *this;
    }
    geometry_builder<T>& make_grid(int nx, int ny)
    {
        for (int y = 0; y < ny; y++) {
            for (int x = 0; x < nx; x++) {
                auto v = std::vector<T> { (T)x, (T)y, (T)0, (T)(x + 1), (T)y, (T)0, (T)(x + 1), (T)(y + 1), (T)0, (T)x, (T)(y + 1), (T)0 };
                tmp.insert(tmp.end(), v.begin(), v.end());
            }
        }
        return *this;
    }
    geometry_builder<T>& transform(const matrix44f& tr)
    {
        for (typename std::vector<T>::size_type i = 0; i < tmp.size(); i += 3) {
            vector3f v3 = tr * vector3f(&tmp[i]);
            v3.copy(&tmp[i]);
        }
        return *this;
    }
    geometry_builder<T>& store()
    {
        data.insert(data.end(), tmp.begin(), tmp.end());
        tmp.clear();
        return *this;
    }
private:
    std::vector<T> data;
    std::vector<T> tmp;
    GLint dimensions;
};

#endif
