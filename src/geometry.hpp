#ifndef _geometry_hpp_
#define _geometry_hpp_

#include <vector>
#include <stack>
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
	geometry(GLsizei count, GLint dimensions);
	~geometry();

	void set_vertex_positions(GLuint positionsId_);
    void set_vertex_tex_coords(GLuint texCoordsId_);
    void set_vertex_normals(GLuint normalsId_);
    void set_vertex_positions(void* data, long size);
	void set_vertex_tex_coords(void* data, long size);
    void set_vertex_normals(void* data, long size);

    GLuint get_positions_id() const;
    GLuint get_tex_coords_id() const;
    GLuint get_normals_id() const;
    GLsizei get_count() const;
    GLint get_dimensions() const;

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
    buffer_object_builder(std::vector<T> data_);
    void* get_data();
    GLsizeiptr get_size();
    GLsizeiptr get_count();
    GLuint build();

private:
    std::vector<T> data;
};

template <class T>
struct geometry_builder {
    geometry_builder(GLint dimensions);
    std::unique_ptr<geometry<T>> build();
    geometry_builder<T>& operator<<(const std::vector<T>& v);
    geometry_builder<T>& make_grid(int nx, int ny);
    geometry_builder<T>& transform(const matrix44f& tr);
    geometry_builder<T>& begin();
    geometry_builder<T>& end();
private:
    std::stack<std::vector<T>> data;
    GLint dimensions;
};

#include "geometry_impl.hpp"

#endif
