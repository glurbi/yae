#include "yae.hpp"
#include "shader.hpp"
#include "sdl.hpp"

void main()
{
    auto engine = std::make_unique<yae::sdl_engine>();
    auto window = engine->create_simple_window();
    auto cv = yae::clipping_volume{ -2.0f, 2.0f, -2.0f, 2.0f, 2.0f, 100.0f };
    window->set_desired_clipping_volume(cv);
    auto on_resize = [&](yae::rendering_context& ctx) {
        window->adapt_camera_to_window_size<yae::perspective_camera_creator, yae::fit_all_adapter>();
    };
    on_resize(yae::rendering_context{});
    window->set_resize_callback(on_resize);
    window->close_when_keydown();

    auto box = yae::make_box<float>(10, 20, 5);
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
        window->get_camera()->render(root, ctx, prog);
    });

    engine->run(window.get());
}
