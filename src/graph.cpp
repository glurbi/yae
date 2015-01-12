#include <cmath>
#include <vector>
#include <memory>
#include <cstring>

#include "matrix.hpp"
#include "graph.hpp"
#include "program.hpp"

using namespace yae;

rendering_context::rendering_context()
{
    memset(last_frame_times_seconds, 0, 100);
    elapsed_time_seconds = 0.0;
    frame_count = 0;
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
    direction_v = normalize(direction_v * (float) cos(to_radians(deg)) + up_v * (float) sin(to_radians(deg)));
    up_v = cross_product(direction_v, right_v) * -1.0f;
}

void camera::rotate_y(float deg)
{
    direction_v = normalize(direction_v * (float) cos(to_radians(deg)) - right_v * (float) sin(to_radians(deg)));
    right_v = cross_product(direction_v, up_v);
}

void camera::rotate_z(float deg)
{
    right_v = normalize(right_v * (float) cos(to_radians(deg)) + up_v * (float) sin(to_radians(deg)));
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

