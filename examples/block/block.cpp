#include <GL/glew.h>

#include "yae.hpp"
#include "sdl.hpp"

std::unique_ptr<yae::camera> create_camera(yae::window* win)
{
    yae::clipping_volume cv;
    int w = win->width();
    int h = win->height();
    float ar = (float)w / h;
    cv.right = 2.0f;
    cv.left = -2.0f;
    cv.bottom = -2.0f / ar;
    cv.top = 2.0f / ar;
    cv.nearp = 2.0f;
    cv.farp = 100.0f;
    std::unique_ptr<yae::camera> camera = std::make_unique<yae::perspective_camera>(cv);
    camera->move_backward(20.0f);
    return camera;
}

void main()
{
    auto yae = yae::sdl_backend{};
    auto engine = yae::engine{};
    auto window = yae.create_simple_window();
    auto camera = create_camera(window.get());

    auto box = yae::make_box<float>(10,20,5);
    auto node = std::make_shared<yae::geometry_node<float>>(std::move(box));
    auto root = std::make_shared<yae::group>();
    root->set_transform_callback([](yae::rendering_context& ctx) {
        float f = (float)ctx.elapsed_time_seconds;
        return yae::multm(
            yae::rotation(10.0f*f, 1.0f, 0.0f, 0.0f),
            yae::rotation(20.0f*f, 0.0f, 1.0f, 0.0f),
            yae::rotation(50.0f*f, 0.0f, 0.0f, 1.0f));
    });
    root->add(node);
    std::shared_ptr<yae::wireframe_program> prog = std::make_shared<yae::wireframe_program>();
    prog->set_solid_color(yae::color4f(0.5f, 0.5f, 0.5f));
    prog->set_wire_color(yae::color4f(1.0f, 1.0f, 1.0f));

    engine.set_render_callback([&](yae::rendering_context& ctx) {
        camera->render(root, ctx, prog);
    });

    engine.set_resize_callback([&](yae::rendering_context& ctx) {
        int w = window->width();
        int h = window->height();
        camera = create_camera(window.get());
        glViewport(0, 0, w, h);
    });

    engine.run(window.get(), yae);
}
