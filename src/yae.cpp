#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>

#include "yae.hpp"

using namespace yae;

struct timer::timer_private {
    static const double r;
    std::chrono::high_resolution_clock::time_point start;
    timer_private() {}
    ~timer_private() {}
};

const double timer::timer_private::r = (double) std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den;

timer::timer()
{
    p = std::unique_ptr<timer::timer_private>(new timer::timer_private());
    reset();
}

timer::~timer()
{
}
    
void timer::reset()
{
    p->start = std::chrono::high_resolution_clock::now();
}

double timer::elapsed()
{
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration duration = now - p->start;
    return duration.count() * p->r;
}

texture::texture(GLubyte* data, GLsizei w, GLsizei h)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

texture::~texture()
{
    glDeleteTextures(1, &id);
}

GLuint texture::get_id() const
{
    return id;
}

void yae::check_for_opengl_errors()
{
    switch (glGetError()) {
    case GL_INVALID_ENUM: std::cout << "GLenum argument out of range" << std::endl; break;
    case GL_INVALID_VALUE: std::cout << "Numeric argument out of range" << std::endl; break;
    case GL_INVALID_OPERATION: std::cout << "Operation illegal in current state" << std::endl; break;
    case GL_STACK_OVERFLOW: std::cout << "Command would cause a stack overflow" << std::endl; break;
    case GL_STACK_UNDERFLOW: std::cout << "Command would cause a stack underflow" << std::endl; break;
    case GL_OUT_OF_MEMORY: std::cout << "Not enough memory left to execute command" << std::endl; break;
    }
}

rendering_context::rendering_context()
{
    memset(last_frame_times_seconds, 0, 100);
    elapsed_time_seconds = 0.0;
    frame_count = 0;
    exit = false;
    reset();
}

void rendering_context::projection(matrix44f mat)
{
    mvp_stack.push_back(multm(mvp_stack.back(), mat));
}

void rendering_context::push(matrix44f mat)
{
    mvp_stack.push_back(multm(mvp_stack.back(), mat));
    mv_stack.push_back(multm(mv_stack.back(), mat));
}

void rendering_context::pop()
{
    mvp_stack.pop_back();
    mv_stack.pop_back();
}

void rendering_context::reset()
{
    mvp_stack.clear();
    mv_stack.clear();
    mvp_stack.push_back(identity<float>());
    mv_stack.push_back(identity<float>());
}

matrix44f rendering_context::mvp()
{
    return mvp_stack.back();
}

matrix44f rendering_context::mv()
{
    return mv_stack.back();
}

camera::camera(const clipping_volume& cv) : cv(cv), position_v(vector3f(0, 0, 0)),
direction_v(vector3f(0, 0, -1)), right_v(vector3f(1, 0, 0)), up_v(vector3f(0, 1, 0))
{
}

void camera::reset()
{
    position_v = vector3f(0, 0, 0);
    direction_v = vector3f(0, 0, -1);
    right_v = vector3f(1, 0, 0);
    up_v = vector3f(0, 1, 0);
}

void camera::rotate_x(float deg)
{
    direction_v = normalize(direction_v * (float)cos(to_radians(deg)) + up_v * (float)sin(to_radians(deg)));
    up_v = cross_product(direction_v, right_v) * -1.0f;
}

void camera::rotate_y(float deg)
{
    direction_v = normalize(direction_v * (float)cos(to_radians(deg)) - right_v * (float)sin(to_radians(deg)));
    right_v = cross_product(direction_v, up_v);
}

void camera::rotate_z(float deg)
{
    right_v = normalize(right_v * (float)cos(to_radians(deg)) + up_v * (float)sin(to_radians(deg)));
    up_v = cross_product(direction_v, right_v) * -1.0f;
}

void camera::move_right(float dist)
{
    position_v = position_v + (right_v * dist);
}

void camera::move_left(float dist)
{
    position_v = position_v - (right_v * dist);
}

void camera::move_up(float dist)
{
    position_v = position_v + (up_v * dist);
}

void camera::move_down(float dist)
{
    position_v = position_v - (up_v * dist);
}

void camera::move_forward(float dist)
{
    position_v = position_v + (direction_v * dist);
}

void camera::move_backward(float dist)
{
    position_v = position_v - (direction_v * dist);
}

void camera::open(float factor)
{
    float delta_height = (cv.top - cv.bottom) * (1 - factor);
    cv.bottom -= delta_height;
    cv.top += delta_height;
    float delta_width = (cv.right - cv.left) * (1 - factor);
    cv.left -= delta_width;
    cv.right += delta_width;
}

void camera::set_opening(float width, float height)
{
    cv.bottom = -height / 2;
    cv.top = height / 2;
    cv.left = -width / 2;
    cv.right = width / 2;
}

float camera::get_height()
{
    return cv.top - cv.bottom;
}

float camera::get_width()
{
    return cv.right - cv.left;
}

matrix44f camera::position_and_orient()
{
    vector3f centerV = position_v + direction_v;
    return look_at(position_v.x(), position_v.y(), position_v.z(), centerV.x(), centerV.y(), centerV.z(), up_v.x(), up_v.y(), up_v.z());
}

perspective_camera::perspective_camera(const clipping_volume& cv)
: camera(cv)
{
}

void perspective_camera::render(std::shared_ptr<node> node, rendering_context& ctx, std::shared_ptr<program> prog)
{
    ctx.projection(frustum(cv.left, cv.right, cv.bottom, cv.top, cv.nearp, cv.farp));
    ctx.push(position_and_orient());
    ctx.prog = prog;
    node->render(ctx);
    ctx.reset();
}

parallel_camera::parallel_camera(const clipping_volume& clippingVolume)
: camera(clippingVolume)
{
}

void parallel_camera::render(std::shared_ptr<node> node, rendering_context& ctx, std::shared_ptr<program> prog)
{
    ctx.projection(ortho(cv.left, cv.right, cv.bottom, cv.top, cv.nearp, cv.farp));
    ctx.push(position_and_orient());
    ctx.prog = prog;
    node->render(ctx);
    ctx.reset();
}

group::group()
: transform_callback([](rendering_context& ctx) { return identity<float>(); })
{
}

void group::set_transform_callback(std::function<matrix44f(rendering_context&)> f)
{
    transform_callback = f;
}

void group::add(std::shared_ptr<node> node)
{
    children.push_back(node);
}

void group::render(rendering_context& ctx)
{
    ctx.push(transform_callback(ctx));
    for (auto child : children) {
        child->render(ctx);
    }
    ctx.pop();
}

rendering_element::rendering_element(std::string name, std::shared_ptr<node> node, std::shared_ptr<program> prog)
    : _name(name), _node(node), _prog(prog) {}

void rendering_element::render(rendering_context& ctx, camera& cam)
{
    cam.render(_node, ctx, _prog);
}

rendering_scene::rendering_scene()
{
}
 
void rendering_scene::add_element(std::shared_ptr<rendering_element> el)
{
    _rendering_elements.push_back(el);
}

void rendering_scene::render(rendering_context& ctx, camera& cam)
{
    for (auto el : _rendering_elements) {
        el->render(ctx, cam);
    }
}

window::window()
{
    set_key_event_callback([&](yae::rendering_context& ctx, yae::event evt) {});
    set_resize_callback([&](yae::rendering_context& ctx) {});
    set_render_callback([&](yae::rendering_context& ctx) {});
    _desired_cv = clipping_volume{ -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f };
    _camera = std::make_shared<parallel_camera>(_desired_cv);
}

void window::associate_scene(std::shared_ptr<rendering_scene> scene)
{
    _scene = scene;
}

void window::set_resize_callback(resize_callback cb)
{
    _resize_cb = cb;
}

void window::set_render_callback(std::function<void(rendering_context&)> cb)
{
    _render_cb = cb;
}

void window::set_key_event_callback(key_event_callback cb)
{
    _key_event_cb = cb;
}

void window::close_when_keydown()
{
    set_key_event_callback([&](yae::rendering_context& ctx, yae::event evt) {
        if (evt.value == keydown()) {
            ctx.exit = true;
        }
    });
}

void window::render(rendering_context& ctx)
{
    make_current();
    for (event e : events()) {
        if (e.value == quit()) {
            ctx.exit = true;
            return;
        } else if (e.value == keydown()) {
            _key_event_cb(ctx, e);
        } else if (e.value == window_resized()) {
            _resize_cb(ctx);
        }
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_cb(ctx);
    _scene->render(ctx, *_camera);
    swap();
}

void engine::run(window* win)
{
    while (!ctx.exit) {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count % 100] = timer_frame.elapsed();
        timer_frame.reset();
        win->render(ctx);
        check_for_opengl_errors();
        ctx.frame_count++;
    }
}

