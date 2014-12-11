#ifndef _program_hpp_
#define _program_hpp_

#include <string>
#include <map>
#include <memory>
#include <GL/glew.h>

#include "matrix.hpp"
#include "geometry.hpp"
#include "texture.hpp"

class rendering_context;

template <class T, GLenum T_primitive>
class geometry;

template <GLenum type>
class shader {
public:
    shader(const std::string& source);
    ~shader();
    GLuint get_id() const;
private:
    GLuint id;
    shader(const shader&);
};

class program {
public:
    program(const std::string& vertex_shader_source,
            const std::string& fragment_shader_source,
            const std::map<int, std::string>& attribute_indices);
    virtual void render(const geometry<float>& geometry, rendering_context& ctx) = 0;
    inline void set_polygon_face(GLenum polygon_face) { this->polygon_face = polygon_face; }
    inline void set_polygon_mode(GLenum polygon_mode) { this->polygon_mode = polygon_mode; }
    ~program();
protected:
    GLuint id;
    GLenum polygon_face; // GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
    GLenum polygon_mode; // GL_POINT, GL_LINE, GL_FILL
private:
    shader<GL_VERTEX_SHADER> vertex_shader;
    shader<GL_FRAGMENT_SHADER> fragment_shader;
    program(const program& that);
};

class monochrome_program : public program {
public:
    monochrome_program(const std::string& monochrome_vert, const std::string& monochrome_frag, const std::map<int, std::string>& attribute_indices);
    virtual void render(const geometry<float>& geometry, rendering_context& ctx);
    inline void set_color(color col) { this->col = col; }
    static std::shared_ptr<monochrome_program> create_2d();
    static std::shared_ptr<monochrome_program> create_3d();
private:
    color col;
};

class texture_program : public program {
public:
    virtual void render(const geometry<float>& geometry, rendering_context& ctx);
    void set_texture(std::shared_ptr<texture> t);
    static std::shared_ptr<texture_program> create();
private:
    texture_program(std::map<int, std::string>& attribute_indices);
    std::shared_ptr<texture> current_texture;
};

class flat_shading_program : public program {
public:
    virtual void render(const geometry<float>& geometry, rendering_context& ctx);
    inline void set_color(color col) { this->col = col; }
    static std::shared_ptr<flat_shading_program> Create();
private:
    flat_shading_program(const std::map<int, std::string>& attribute_indices);
    color col;
};

#endif
