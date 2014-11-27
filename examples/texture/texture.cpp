#include <iostream>
#include <memory>
#include <iostream>
#include <numeric>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <vector>
#include <set>
#include <memory>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>

#include "yae.hpp"

int main()
{
    auto yae = yae::yae{};
    auto engine = yae::engine{};
    auto hero_image = sf::Image{};
    hero_image.loadFromFile("smiley.png");
    hero_image.flipVertically();
    auto pixels = (GLubyte*)hero_image.getPixelsPtr();
    auto width = hero_image.getSize().x;
    auto height = hero_image.getSize().y;
    auto hero_texture = std::make_shared<texture>(pixels, width, height);

    buffer_object_builder<float> b({ -50.0f, -50.0f, 50.0f, -50.0f, 50.0f, 50.0f, -50.0f, 50.0f });
    auto multi_hero = std::make_shared<geometry<float>>(b.get_size() / 2);
    multi_hero->set_vertex_positions(b.build());
    multi_hero->set_vertex_tex_coords(b.build());
    auto node = std::make_shared<geometry_node<float>>(multi_hero);
    auto cam_height = engine.get_camera().get_height();
    auto cam_width = engine.get_camera().get_width();
    auto root = std::make_shared<group>();
    root->add(node);
    auto textureProgram = texture_program::create();
    textureProgram->set_texture(hero_texture);

    engine.set_callback([&] (rendering_context& ctx) {
        engine.get_camera().rotate_z(0.5);
        auto f = (float)(1.0+0.5*sin(0.1*2.0*3.1415927*ctx.elapsed_time_seconds));
        engine.get_camera().set_opening(cam_width * f, cam_height * f);
        engine.get_camera().render(root, ctx, textureProgram);
    });

    engine.run();
}
