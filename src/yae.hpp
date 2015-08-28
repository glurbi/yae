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
#include "shader.hpp"

namespace yae {

void check_for_opengl_errors();

class rendering_context;
class program;
class shader_program;
struct window;
    
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

struct viewport_relative {
    float x_percent;
    float y_percent;
    float width_percent;
    float height_percent;
};

struct viewport {
    int x;
    int y;
    int w;
    int h;
};

// cf http://www.codecolony.de/opengl.htm#camera2
struct camera {
    camera(const clipping_volume& cv);
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

struct rendering_element {
    rendering_element(std::string name);
    virtual void render(rendering_context& ctx) = 0;
protected:
    std::string _name;
};

struct node_rendering_element : public rendering_element {
    node_rendering_element(
        std::string name,
        std::shared_ptr<node> node,
        std::shared_ptr<program> prog,
        std::shared_ptr<camera> _camera
    );
    virtual void render(rendering_context& ctx);
private:
    std::shared_ptr<node> _node;
    std::shared_ptr<program> _prog;
    std::shared_ptr<camera> _camera;
};

struct custom_rendering_element : public rendering_element {
    typedef std::function<void(rendering_context&)> callback;
    custom_rendering_element(
        std::string name,
        callback cb
    );
    virtual void render(rendering_context& ctx);
private:
    callback _callback;
};

custom_rendering_element::callback clear_viewport_callback(color4f c, viewport& vp);

struct rendering_scene {
    rendering_scene();
    void add_element(std::shared_ptr<rendering_element> el);
    void render(rendering_context& ctx);

    struct fit_width_adapter {
        static clipping_volume adapt(clipping_volume cv, float wh_ratio);
    };

    struct fit_height_adapter {
        static clipping_volume adapt(clipping_volume cv, float wh_ratio);
    };

    struct fit_all_adapter {
        static clipping_volume adapt(clipping_volume cv, float wh_ratio);
    };

    struct ignore_ratio_adapter {
        static clipping_volume adapt(clipping_volume cv, float wh_ratio);
    };

    template<class ClippingVolumeAdapter = fit_all_adapter>
    void associate_camera(std::shared_ptr<camera> cam, window* win, viewport_relative vpr);

    inline viewport& get_viewport() { return _viewport; }

private:
    std::vector<std::shared_ptr<rendering_element>> _rendering_elements;
    clipping_volume _desired_cv;
    viewport _viewport;    
    std::shared_ptr<camera> _camera;
};

struct window {

    typedef std::function<void(rendering_context&)> resize_callback;
    typedef std::function<void(rendering_context&)> render_callback;
    typedef std::function<void(rendering_context&, event&)> key_event_callback;

    window();
    virtual int width() = 0;
    virtual int height() = 0;
    virtual void swap() = 0;
    virtual void make_current() = 0;
    virtual std::vector<event> events() = 0;
    virtual int quit() = 0;
    virtual int keydown() = 0;
    virtual int window_resized() = 0;
    void close_when_keydown();
    void add_resize_callback(resize_callback f);
    void set_render_callback(render_callback f);
    void set_key_event_callback(key_event_callback f);
    void add_scene(std::shared_ptr<rendering_scene> scene);
    void render(rendering_context& ctx);
    
private:
    std::vector<resize_callback> _resize_callbacks;
    render_callback _render_cb;
    key_event_callback _key_event_cb;
    std::vector<std::shared_ptr<rendering_scene>> _scenes;
};

template<class ClippingVolumeAdapter>
void rendering_scene::associate_camera(std::shared_ptr<camera> cam, window* win, viewport_relative vpr)
{
    _camera = cam;
    _desired_cv = _camera->cv;
    auto cb = [=](yae::rendering_context& ctx) {
        int win_w = win->width();
        int win_h = win->height();
        _viewport.x = static_cast<int>(vpr.x_percent * win_w);
        _viewport.y = static_cast<int>(vpr.y_percent * win_h);
        _viewport.w = static_cast<int>(vpr.width_percent * win_w);
        _viewport.h = static_cast<int>(vpr.height_percent * win_h);
        float ar = (float)_viewport.w / _viewport.h;
        _camera->cv = ClippingVolumeAdapter::adapt(_desired_cv, ar);
    };
    win->add_resize_callback(cb);
    auto ctx = yae::rendering_context();
    cb(ctx);
}

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
