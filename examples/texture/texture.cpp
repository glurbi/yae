#include "yae.hpp"
#include "shader.hpp"
#include "sdl.hpp"

int main()
{
    auto engine = std::make_unique<yae::sdl_engine>();
    auto window = engine->create_simple_window();
    auto cv = yae::clipping_volume{ -8.0f, 8.0f, -6.0f, 6.0f, 1.0f, -1.0f };
    auto cam = std::make_shared<yae::parallel_camera>(cv);
    window->associate_camera<yae::window::fit_all_adapter>(cam);
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
    auto re = std::make_shared<yae::rendering_element>("smiley_canvas", root, prog);
    scene->add_element(re);

    window->set_render_callback([&](yae::rendering_context& ctx) {
        cam->rotate_z(0.5);
    });

    window->associate_scene(scene);

    engine->run(window.get());

    return 0;
}
