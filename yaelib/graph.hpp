#ifndef _graph_hpp_
#define _graph_hpp_

#include <math.h>
#include <vector>
#include <memory>

#include "matrix.hpp"
#include "geometry.hpp"
#include "texture.hpp"
#include "program.hpp"
#include "context.hpp"

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
    matrix44 position_and_orient();
    vector3 position_v;
    vector3 direction_v;
    vector3 right_v;
    vector3 up_v;
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
    void transformation(const matrix44& tr);
    void add(std::shared_ptr<node> node);
    virtual void render(rendering_context& ctx);
protected:
    std::vector<std::shared_ptr<node>> children;
    matrix44 transform;
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

#endif
