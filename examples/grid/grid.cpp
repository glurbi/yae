#include <GL/glew.h>
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
    auto grid = make_grid<float>(10,20);

    auto node = std::make_shared<geometry_node<float>>(std::move(grid));
    auto root = std::make_shared<group>();
    root->add(node);
    auto monochrome_program = monochrome_program::create();

    engine.set_render_callback([&](rendering_context& ctx) {
        camera->render(root, ctx, monochrome_program);
    });

    engine.run(*window);
}
