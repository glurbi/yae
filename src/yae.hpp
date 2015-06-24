#ifndef _yae_hpp_
#define _yae_hpp_

#include <memory>
#include <map>
#include <cmath>
#include <vector>
#include <functional>

#include <GL/glew.h>

#include "matrix.hpp"
#include "geometry.hpp"

namespace yae {

void check_for_opengl_errors();

class rendering_context;
class shader_program;
    
class timer {
public:
    timer();
    ~timer();
    void reset();
    double elapsed();
private:
    struct timer_private;
    std::unique_ptr<timer_private> p;
};

class texture {
public:
    texture(GLubyte* data, GLsizei w, GLsizei h);
    ~texture();
    GLuint get_id() const;
private:
    GLuint id;
};

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

class rendering_context {
public:
    rendering_context();
    void projection(matrix44f mat);
    void push(matrix44f mat);
    void pop();
    matrix44f mvp();
    matrix44f mv();
    void reset();
    vector3f dir;
    double elapsed_time_seconds;
    double last_frame_times_seconds[100];
    long frame_count;
    std::shared_ptr<program> prog;
private:
    std::vector<matrix44f> mvp_stack;
    std::vector<matrix44f> mv_stack;
};

class node {
public:
    virtual void render(rendering_context& ctx) = 0;
};

struct clipping_volume {
    float left;
    float right;
    float bottom;
    float top;
    float nearp;
    float farp;
};

// cf http://www.codecolony.de/opengl.htm#camera2
struct camera {
    camera(const clipping_volume& clippingVolume);
    virtual void render(std::shared_ptr<node> node, rendering_context& ctx, std::shared_ptr<program> program) = 0;
    void reset();
    void rotate_x(float deg);
    void rotate_y(float deg);
    void rotate_z(float deg);
    void move_right(float dist);
    void move_left(float dist);
    void move_up(float dist);
    void move_down(float dist);
    void move_forward(float dist);
    void move_backward(float dist);
    void open(float factor);
    void set_opening(float width, float height);
    float get_height();
    float get_width();
    matrix44f position_and_orient();
    vector3f position_v;
    vector3f direction_v;
    vector3f right_v;
    vector3f up_v;
    clipping_volume cv;
};

class perspective_camera : public camera {
public:
    perspective_camera(const clipping_volume& cv);
    virtual void render(std::shared_ptr<node> node, rendering_context& ctx, std::shared_ptr<program> program);
};

class parallel_camera : public camera {
public:
    parallel_camera(const clipping_volume& cv);
    virtual void render(std::shared_ptr<node> node, rendering_context& ctx, std::shared_ptr<program> program);
};

class group : public node {
public:
    group();
    void set_transform_callback(std::function<matrix44f(rendering_context&)> f);
    void add(std::shared_ptr<node> node);
    virtual void render(rendering_context& ctx);
protected:
    std::vector<std::shared_ptr<node>> children;
    std::function<matrix44f(rendering_context&)> transform_callback;
};

template<class T>
class geometry_node : public node {
public:
    geometry_node(std::shared_ptr<geometry<T>> geom) : geom(geom) {}
    virtual void render(rendering_context& ctx) {
        ctx.prog->render(*geom, ctx);
    }
private:
    std::shared_ptr<geometry<T>> geom;
};

struct event {
    event(int v) : value(v) {}
    int value;
};

struct window {
    virtual int width() = 0;
    virtual int height() = 0;
    virtual void swap() = 0;
    virtual std::vector<event> events() = 0;
    virtual std::unique_ptr<camera> create_perspective_camera(const clipping_volume& cv);
    virtual std::unique_ptr<camera> create_parallel_camera(const clipping_volume& cv);
    void set_resize_callback(std::function<void(rendering_context&)> f);
    std::function<void(rendering_context&)> resize_callback;
};

struct engine {
    void run(window* win);
    void set_render_callback(std::function<void(rendering_context&)> f);
    virtual std::unique_ptr<window> create_simple_window() = 0;
    virtual int quit() = 0;
    virtual int keydown() = 0;
    virtual int window_resized() = 0;
private:
    timer timer_absolute;
    timer timer_frame;
    rendering_context ctx;
    std::function<void(rendering_context&)> render_callback;
};

}

#endif
