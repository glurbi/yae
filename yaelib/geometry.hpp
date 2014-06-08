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

template<class T>
class geometry {

public:
	geometry(GLsizei count_) :
        count(count_), positions_id(0), tex_coords_id(0), normals_id(0) {}
	
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

private:
	GLuint positions_id;
	GLuint tex_coords_id;
	GLuint normals_id;
    GLsizei count;

};

template <class T>
class buffer_object_builder {

public:

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

    buffer_object_builder<T>& operator<<(T t) {
        data.push_back(t);
        return *this;
    }

private:

    std::vector<T> data;

};

#endif
