#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include "yae.hpp"

std::unique_ptr<camera> create_camera(sf::RenderWindow& window)
{
    clipping_volume cv;
    int div = 100;
    cv.right = (float)window.getSize().x / div;
    cv.left = (float)-(int)window.getSize().x / div;
    cv.bottom = (float)-(int)window.getSize().y / div;
    cv.top = (float)window.getSize().y / div;
    cv.nearp = 1.0f;
    cv.farp = -1.0f;
    return std::make_unique<parallel_camera>(cv);
}

std::unique_ptr<sf::RenderWindow> create_window()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    settings.depthBits = 16;
    auto window = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 600), "", sf::Style::Default, settings);
    window->setVerticalSyncEnabled(true);
    window->setMouseCursorVisible(false);
    glewInit();
    glViewport(0, 0, window->getSize().x, window->getSize().y);
    return window;
}

int main()
{
    auto yae = yae::yae{};
    yae::engine engine {};
    auto window = create_window();
    auto camera = create_camera(*window);
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
    auto cam_height = camera->get_height();
    auto cam_width = camera->get_width();
    auto root = std::make_shared<group>();
    root->add(node);
    auto textureProgram = texture_program::create();
    textureProgram->set_texture(hero_texture);

    engine.set_render_callback([&] (rendering_context& ctx) {
        camera->rotate_z(0.5);
        auto f = (float)(1.0+0.5*sin(0.1*2.0*3.1415927*ctx.elapsed_time_seconds));
        camera->set_opening(cam_width * f, cam_height * f);
        camera->render(root, ctx, textureProgram);
    });

    engine.set_resize_callback([&](rendering_context& ctx, sf::Event& event) {
        camera = create_camera(*window);
        glViewport(0, 0, event.size.width, event.size.height);
        sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
        window->setView(view);
    });

    engine.run(*window);
}
