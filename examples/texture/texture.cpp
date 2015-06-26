#include "yae.hpp"
#include "shader.hpp"
#include "sdl.hpp"

void main()
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
    auto cam_height = cam->get_height();
    auto cam_width = cam->get_width();
    auto root = std::make_shared<yae::group>();
    root->add(node);
    auto texture_program = yae::texture_program::create();
    texture_program->set_texture(hero_texture);

    window->set_render_callback([&](yae::rendering_context& ctx) {
        cam->rotate_z(0.5);
        auto f = (float)(1.0+0.5*sin(0.1*2.0*3.1415927*ctx.elapsed_time_seconds));
        cam->set_opening(cam_width * f, cam_height * f);
        cam->render(root, ctx, texture_program);
    });

    engine->run(window.get());
}
