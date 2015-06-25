#include "yae.hpp"
#include "shader.hpp"
#include "sdl.hpp"

void main()
{
    auto engine = std::make_unique<yae::sdl_engine>();
    auto window = engine->create_simple_window();
    auto cv = yae::clipping_volume{ -2.0f, 2.0f, -2.0f, 2.0f, 2.0f, 100.0f };
    auto camera = window->create_perspective_camera(cv);

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
    auto prog = std::make_shared<yae::wireframe_program>();
    prog->set_solid_color(yae::color4f(0.5f, 0.5f, 0.5f));
    prog->set_wire_color(yae::color4f(1.0f, 1.0f, 1.0f));

    window->set_render_callback([&](yae::rendering_context& ctx) {
        camera->render(root, ctx, prog);
    });

    window->set_resize_callback([&](yae::rendering_context& ctx) {
        int w = window->width();
        int h = window->height();
        camera = window->create_perspective_camera(cv);
        glViewport(0, 0, w, h);
    });

    window->close_when_keydown();

    engine->run(window.get());
}
