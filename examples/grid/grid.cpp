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
    std::unique_ptr<camera> camera = std::make_unique<perspective_camera>(cv);
    camera->move_backward(5.0f);
    return camera;
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

    auto geomb = geometry_builder<float>{3};
    geomb.begin();
    geomb.begin().make_grid(10,20).end();
    geomb.begin().make_grid(5, 20).transform(rotation(90.0f, 0.0f, 1.0f, 0.0f)).end();
    geomb.begin().make_grid(5, 20).transform(rotation(90.0f, 0.0f, 1.0f, 0.0f)).transform(translation(10.0f, 0.0f, 0.0f)).end();
    geomb.begin().make_grid(10, 20).transform(translation(0.0f, 0.0f, -5.0f)).end();
    geomb.begin().make_grid(10, 5).transform(rotation(-90.0f, 1.0f, 0.0f, 0.0f)).end();
    geomb.begin().make_grid(10, 5).transform(rotation(-90.0f, 1.0f, 0.0f, 0.0f)).transform(translation(0.0f, 20.0f, 0.0f)).end();
    geomb.transform(translation(-5.0f, -10.0f, 2.5f));
    geomb.end();
    auto grid = geomb.build();

    auto node = std::make_shared<geometry_node<float>>(std::move(grid));
    auto root = std::make_shared<group>();
    root->transformation(translation(0.0f, 0.0f, 0.0f));
    root->add(node);
    auto monochrome_program = monochrome_program::create_3d();
    monochrome_program->set_polygon_mode(GL_LINE);
    monochrome_program->set_polygon_face(GL_FRONT_AND_BACK);

    engine.set_render_callback([&](rendering_context& ctx) {
        camera->render(root, ctx, monochrome_program);
    });

    engine.set_resize_callback([&](rendering_context& ctx, sf::Event& event) {
        camera = create_camera(*window);
        glViewport(0, 0, event.size.width, event.size.height);
        sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
        window->setView(view);
    });

    engine.run(*window);
}
