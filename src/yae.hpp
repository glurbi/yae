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
class program;
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
    bool exit;
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
    window();
    virtual int width() = 0;
    virtual int height() = 0;
    virtual void swap() = 0;
    virtual void make_current() = 0;
    virtual std::vector<event> events() = 0;
    virtual int quit() = 0;
    virtual int keydown() = 0;
    virtual int window_resized() = 0;
    virtual std::unique_ptr<camera> create_perspective_camera(const clipping_volume& cv);
    virtual std::unique_ptr<camera> create_parallel_camera(const clipping_volume& cv);
    void close_when_keydown();
    void set_resize_callback(std::function<void(rendering_context&)> f);
    void set_render_callback(std::function<void(rendering_context&)> f);
    void set_key_event_callback(std::function<void(rendering_context&, event&)> f);
    void render(rendering_context& ctx);
private:
    std::function<void(rendering_context&)> resize_callback;
    std::function<void(rendering_context&)> render_callback;
    std::function<void(rendering_context&, event&)> key_event_callback;
};

struct engine {
    void run(window* win);
    virtual std::unique_ptr<window> create_simple_window() = 0;
private:
    timer timer_absolute;
    timer timer_frame;
    rendering_context ctx;
};

}

#endif
