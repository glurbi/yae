#include "yae.hpp"
#include "shader.hpp"
#include "sdl.hpp"

int main()
{
    auto engine = std::make_unique<yae::sdl_engine>();
    auto window = engine->create_simple_window();
    auto cv = yae::clipping_volume{ -2.0f, 2.0f, -2.0f, 2.0f, 2.0f, 100.0f };
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

    auto scene = std::make_shared<yae::rendering_scene>();
    auto cam = std::make_shared<yae::perspective_camera>(cv);
    cam->move_backward(20.0f);
    scene->associate_camera<yae::rendering_scene::fit_all_adapter>(cam, window.get(), yae::viewport{0.0f, 0.0f, 0.5f, 0.5f});
    auto prepare_cb = yae::create_prepare_callback(yae::color4f(1.0f, 0.0f, 0.0f, 0.0f));
    auto cre = std::make_shared<yae::custom_rendering_element>("clear_buffer", prepare_cb);
    scene->add_element(cre);
    auto nre = std::make_shared<yae::node_rendering_element>("wireframe_block", root, prog, cam);
    scene->add_element(nre);
    
    window->add_scene(scene);

    engine->run(window.get());

    return 0;
}
