#include <array>

#include "yae.hpp"
#include "shader.hpp"
#include "sdl.hpp"

int main()
{
    auto engine = std::make_unique<yae::sdl_engine>();
    auto window = engine->create_simple_window();
    auto cv = yae::clipping_volume{ -2.0f, 2.0f, -2.0f, 2.0f, 2.0f, 100.0f };
    window->close_when_keydown();

    auto box = yae::make_box<float>(10, 20, 5).build();
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

    std::array<yae::color4f, 4> bg_colors = {
        yae::color4f{ 1.0f, 0.0f, 0.0f, 0.0f },
        yae::color4f{ 0.0f, 1.0f, 0.0f, 0.0f },
        yae::color4f{ 0.0f, 0.0f, 0.0f, 0.0f },
        yae::color4f{ 0.0f, 0.0f, 1.0f, 0.0f },
    };
    std::array<yae::viewport_relative, 4> viewports = {
        yae::viewport_relative{ 0.0f, 0.0f, 0.5f, 0.5f },
        yae::viewport_relative{ 0.5f, 0.0f, 0.5f, 0.5f },
        yae::viewport_relative{ 0.5f, 0.5f, 0.5f, 0.5f },
        yae::viewport_relative{ 0.0f, 0.5f, 0.5f, 0.5f }
    };
    for (int i : { 0, 1, 2, 3 }) {
        auto prog = std::make_shared<yae::wireframe_program>();
        prog->set_solid_color(yae::color4f(0.5f, 0.5f, 0.5f));
        prog->set_wire_color(yae::color4f(1.0f, 1.0f, 1.0f));
        auto cam = std::make_shared<yae::perspective_camera>(cv);
        auto scene = std::make_shared<yae::rendering_scene>();
        switch (i) {
        case 0:
            scene->associate_camera<yae::rendering_scene::fit_all_adapter>(cam, window.get(), viewports[i]);
            break;
        case 1:
            scene->associate_camera<yae::rendering_scene::fit_height_adapter>(cam, window.get(), viewports[i]);
            break;
        case 2:
            scene->associate_camera<yae::rendering_scene::fit_width_adapter>(cam, window.get(), viewports[i]);
            break;
        case 3:
            scene->associate_camera<yae::rendering_scene::ignore_ratio_adapter>(cam, window.get(), viewports[i]);
            break;
        }
        auto clear_viewport_cb = yae::clear_viewport_callback(bg_colors[i], scene->get_viewport());
        auto cre = std::make_shared<yae::custom_rendering_element>("clear_viewport", clear_viewport_cb);
        auto nre = std::make_shared<yae::node_rendering_element>("wireframe_block", root, prog, cam);
        scene->add_element(cre);
        scene->add_element(nre);
        cam->move_backward(20.0f);
        window->add_scene(scene);
    }

    engine->run(window.get());

    return 0;
}
