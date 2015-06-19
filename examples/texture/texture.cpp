#include <GL/glew.h>
#include "yae.hpp"

std::unique_ptr<yae::camera> create_camera(SDL_Window* window)
{
    yae::clipping_volume cv;
    int div = 100;
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    cv.right = (float)w / div;
    cv.left = (float)-(int)w / div;
    cv.bottom = (float)-(int)h / div;
    cv.top = (float)h / div;
    cv.nearp = 1.0f;
    cv.farp = -1.0f;
    return std::make_unique<yae::parallel_camera>(cv);
}

void main()
{
    auto yae = yae::yae{};
    auto engine = yae::engine{};
    auto window = yae::create_simple_window();
    auto camera = create_camera(window);

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
    auto cam_height = camera->get_height();
    auto cam_width = camera->get_width();
    auto root = std::make_shared<yae::group>();
    root->add(node);
    auto texture_program = yae::texture_program::create();
    texture_program->set_texture(hero_texture);

    engine.set_render_callback([&](yae::rendering_context& ctx) {
        camera->rotate_z(0.5);
        auto f = (float)(1.0+0.5*sin(0.1*2.0*3.1415927*ctx.elapsed_time_seconds));
        camera->set_opening(cam_width * f, cam_height * f);
        camera->render(root, ctx, texture_program);
    });

    engine.set_resize_callback([&](yae::rendering_context& ctx, SDL_Event event) {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        std::cout << "FFOO" << std::endl;
        camera = create_camera(window);
        glViewport(0, 0, w, h);
    });

    engine.run(window);
}
