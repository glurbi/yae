#include "yae.hpp"
#include "shader.hpp"
#include "sdl.hpp"

int main()
{
    auto engine = std::make_unique<yae::sdl_engine>();
    auto window = engine->create_simple_window();
    auto cv = yae::clipping_volume{ -8.0f, 8.0f, -6.0f, 6.0f, 1.0f, -1.0f };
    window->close_when_keydown();

    auto rwop = SDL_RWFromFile("smiley.png", "rb");
    auto hero_image = IMG_LoadPNG_RW(rwop);
    auto pixels = (GLubyte*)hero_image->pixels;
    auto width = hero_image->w;
    auto height = hero_image->h;
    auto hero_texture = std::make_shared<yae::texture>(pixels, width, height);

    yae::buffer_object_builder<float> b({ -50.0f, -50.0f, 50.0f, -50.0f, 50.0f, 50.0f, -50.0f, 50.0f });
    auto multi_hero = std::make_shared<yae::geometry<float>>(b.get_size() / 2, 2);
    multi_hero->set_vertex_positions(b.build());
    multi_hero->set_vertex_tex_coords(b.build());
    auto node = std::make_shared<yae::geometry_node<float>>(multi_hero);
    auto root = std::make_shared<yae::group>();
    root->add(node);
    auto prog = yae::texture_program::create();
    prog->set_texture(hero_texture);

    auto scene = std::make_shared<yae::rendering_scene>();
    auto cam = std::make_shared<yae::parallel_camera>(cv);
    auto nre = std::make_shared<yae::node_rendering_element>("smiley_canvas", root, prog, cam);
    scene->add_element(nre);
    scene->associate_camera<yae::rendering_scene::fit_all_adapter>(cam, window.get(), yae::viewport_relative{0.0f, 0.0f, 1.0f, 1.0f});

    window->set_render_callback([&](yae::rendering_context& ctx) {
        cam->rotate_z(0.5);
    });

    window->add_scene(scene);

    engine->run(window.get());

    return 0;
}
