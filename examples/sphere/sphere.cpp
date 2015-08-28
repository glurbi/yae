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

    auto transform_fun = [](yae::rendering_context& ctx) {
        float f = (float)ctx.elapsed_time_seconds;
        return yae::multm(
            yae::rotation(10.0f*f, 1.0f, 0.0f, 0.0f),
            yae::rotation(20.0f*f, 0.0f, 1.0f, 0.0f),
            yae::rotation(50.0f*f, 0.0f, 0.0f, 1.0f));
    };

    auto uvsphere = yae::make_uv_sphere<float>(50, 10).build();
    auto octasphere = yae::make_octahedron_sphere<float>(3).build();
    auto uvnode = std::make_shared<yae::geometry_node<float>>(std::move(uvsphere));
    auto octanode = std::make_shared<yae::geometry_node<float>>(std::move(octasphere));
    auto uvgroup = std::make_shared<yae::group>();
    uvgroup->set_transform_callback(transform_fun);
    uvgroup->add(uvnode);
    auto octagroup = std::make_shared<yae::group>();
    octagroup->set_transform_callback(transform_fun);
    octagroup->add(octanode);

    auto root = std::make_shared<yae::group>();
    auto leftgroup = std::make_shared<yae::group>();
    leftgroup->set_transform_callback([](yae::rendering_context& ctx) { return yae::translation(-1.5f, 0.0f, 0.0f); });
    leftgroup->add(uvgroup);
    auto rightgroup = std::make_shared<yae::group>();
    rightgroup->set_transform_callback([](yae::rendering_context& ctx) { return yae::translation(1.5f, 0.0f, 0.0f); });
    rightgroup->add(octagroup);
    root->add(leftgroup);
    root->add(rightgroup);

    auto prog = std::make_shared<yae::wireframe_program>();
    prog->set_solid_color(yae::color4f(0.5f, 0.5f, 0.5f));
    prog->set_wire_color(yae::color4f(1.0f, 1.0f, 1.0f));
    auto cam = std::make_shared<yae::perspective_camera>(cv);
    auto scene = std::make_shared<yae::rendering_scene>();
    scene->associate_camera<yae::rendering_scene::fit_all_adapter>(cam, window.get(), yae::viewport_relative{ 0.0f, 0.0f, 1.0f, 1.0f });
    auto clear_viewport_cb = yae::clear_viewport_callback(yae::color4f{ 0.0f, 0.0f, 0.0f, 0.0f }, scene->get_viewport());
    auto cre = std::make_shared<yae::custom_rendering_element>("clear_viewport", clear_viewport_cb);
    auto nre = std::make_shared<yae::node_rendering_element>("wireframe_sphere", root, prog, cam);
    scene->add_element(cre);
    scene->add_element(nre);
    cam->move_backward(3.0f);
    window->add_scene(scene);

    engine->run(window.get());

    return 0;
}
