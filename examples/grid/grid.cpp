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
    cv.farp = 10.0f;
    return std::make_unique<perspective_camera>(cv);
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
    auto engine = yae::engine{};
    auto window = create_window();
    auto camera = create_camera(*window);
    auto grid = make_grid<float>(10,20);

    auto node = std::make_shared<geometry_node<float>>(std::move(grid));
    auto root = std::make_shared<group>();
    root->transformation(translation(0.0f, 0.0f, 0.0f));
    root->add(node);
    auto monochrome_program = monochrome_program::create();

    engine.set_render_callback([&](rendering_context& ctx) {
        camera->render(root, ctx, monochrome_program);
    });

    engine.set_resize_callback([&](rendering_context& ctx, sf::Event& event) {
        camera = create_camera(*window);
        camera->move_backward(2.0f);
        glViewport(0, 0, event.size.width, event.size.height);
        sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
        window->setView(view);
    });

    engine.run(*window);
}
