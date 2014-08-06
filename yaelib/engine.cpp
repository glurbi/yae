#include <GL/glew.h>
#include <SFML/Graphics.hpp>

#include "engine.hpp"
#include "timer.hpp"
#include "context.hpp"
#include "misc.hpp"

struct yae::engine::impl {
    sf::RenderWindow* window;
    timer timer_absolute;
    timer timer_frame;
    rendering_context ctx;
    camera* mycamera;
    std::function<void (rendering_context&)> callback;
    void init();
    void run();
};

yae::engine::engine() {
    pimpl = std::unique_ptr<impl>(new impl());
}

yae::engine::~engine() {

}

void yae::engine::init() {
    pimpl->init();
}

void yae::engine::run() {
    pimpl->run();
}

camera& yae::engine::get_camera() {
    return *pimpl->mycamera;
}

void yae::engine::set_callback(std::function<void (rendering_context&)> f) {
    pimpl->callback = f;
}

camera* create_camera(sf::RenderWindow& window) {
    clipping_volume cv;
    int div = 100;
    cv.right = (float)window.getSize().x / div;
    cv.left = (float)-(int)window.getSize().x / div;
    cv.bottom = (float)-(int)window.getSize().y / div;
    cv.top = (float)window.getSize().y / div;
    cv.nearp = 1.0f;
    cv.farp = -1.0f;
    return new parallel_camera(cv);
}

void yae::engine::impl::init() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    settings.depthBits = 16;
    window = new sf::RenderWindow(sf::VideoMode(800, 600), "Texture", sf::Style::Default, settings);
    window->setVerticalSyncEnabled(true);
    window->setMouseCursorVisible(false);
    glewInit();
    glViewport(0, 0, window->getSize().x, window->getSize().y);
    ctx.frame_count = 0;
    mycamera = create_camera(*window);
}

void yae::engine::impl::run() {
    while (true) {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count % 100] = timer_frame.elapsed();
        timer_frame.reset();
        check_for_opengl_errors();
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return;
            }
            if (event.type == sf::Event::Resized) {
                mycamera = create_camera(*window);
                glViewport(0, 0, event.size.width, event.size.height);
                sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
                window->setView(view);
            }
            if (event.type == sf::Event::KeyPressed) {
                return;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        callback(ctx);
        window->display();
        ctx.frame_count++;
    }
}
