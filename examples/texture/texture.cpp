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
#include "misc.hpp"
#include "timer.hpp"
#include "matrix.hpp"
#include "graph.hpp"
#include "program.hpp"
#include "geometry.hpp"
#include "timer.hpp"
#include "context.hpp"
#include "engine.hpp"

int main() {
    yae::yae yae;
    yae::engine engine;
    engine.init();

    sf::Image heroImage;
    heroImage.loadFromFile("smiley.png");
    heroImage.flipVertically();
    auto heroTexture = std::make_shared<texture>((GLubyte*)heroImage.getPixelsPtr(), heroImage.getSize().x, heroImage.getSize().y);

    buffer_object_builder<float> b;
    b << -50.0f << -50.0f;
    b << 50.0f << -50.0f;
    b << 50.0f << 50.0f;
    b << -50.0f << 50.0f;
    auto multi_hero = std::make_shared<geometry<float>>(geometry<float>(b.get_size() / 2));
    multi_hero->set_vertex_positions(b.build());
    multi_hero->set_vertex_tex_coords(b.build());

    std::shared_ptr<geometry_node<float>> node = std::make_shared<geometry_node<float>>(geometry_node<float>(multi_hero));

    float cam_height = engine.get_camera().get_height();
    float cam_width = engine.get_camera().get_width();
    auto root = std::make_shared<group>(group());
    root->add(node);
    std::shared_ptr<texture_program> textureProgram = texture_program::create();
    textureProgram->set_texture(heroTexture);

    engine.set_callback([&] (rendering_context& ctx) {
        engine.get_camera().rotate_z(0.5);
        float f = (float)(1.0+0.5*sin(0.1*2.0*3.1415927*ctx.elapsed_time_seconds));
        engine.get_camera().set_opening(cam_width * f, cam_height * f);
        engine.get_camera().render(root, ctx, textureProgram);
    });

    engine.run();
}
