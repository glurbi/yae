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
    virtual void render(const geometry<float>& geometry, rendering_context& ctx) = 0;
};

class composite_program : public program {
};

class shader_program : public program {
public:
    shader_program(const std::string& vertex_shader_source,
            const std::string& fragment_shader_source,
            const std::map<int, std::string>& attribute_indices);
    virtual void render(const geometry<float>& geometry, rendering_context& ctx) = 0;
    inline void set_polygon_face(GLenum polygon_face) { this->polygon_face = polygon_face; }
    inline void set_polygon_mode(GLenum polygon_mode) { this->polygon_mode = polygon_mode; }
    ~shader_program();
protected:
    GLuint id;
    GLenum polygon_face; // GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
    GLenum polygon_mode; // GL_POINT, GL_LINE, GL_FILL
private:
    shader<GL_VERTEX_SHADER> vertex_shader;
    shader<GL_FRAGMENT_SHADER> fragment_shader;
    shader_program(const shader_program& that);
};

class monochrome_program : public shader_program {
public:
    monochrome_program(const std::string& monochrome_vert, const std::string& monochrome_frag, const std::map<int, std::string>& attribute_indices);
    virtual void render(const geometry<float>& geometry, rendering_context& ctx);
    inline void set_color(color4f col) { this->col = col; }
    static std::shared_ptr<monochrome_program> create_2d();
    static std::shared_ptr<monochrome_program> create_3d();
private:
    color4f col;
};

class texture_program : public shader_program {
public:
    virtual void render(const geometry<float>& geometry, rendering_context& ctx);
    void set_texture(std::shared_ptr<texture> t);
    static std::shared_ptr<texture_program> create();
private:
    texture_program(std::map<int, std::string>& attribute_indices);
    std::shared_ptr<texture> current_texture;
};

class flat_shading_program : public shader_program {
public:
    virtual void render(const geometry<float>& geometry, rendering_context& ctx);
    inline void set_color(color4f col) { this->col = col; }
    static std::shared_ptr<flat_shading_program> create();
private:
    flat_shading_program(const std::map<int, std::string>& attribute_indices);
    color4f col;
};

class wireframe_program : public composite_program {
public:
    wireframe_program();
    virtual void render(const geometry<float>& geometry, rendering_context& ctx);
    inline void set_solid_color(color4f c) { this->solid_col = c; }
    inline void set_wire_color(color4f c) { this->wire_col = c; }
private:
    color4f solid_col;
    color4f wire_col;
    std::shared_ptr<monochrome_program> prog;
};

#endif
