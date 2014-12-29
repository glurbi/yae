#include <GL/glew.h>
#include <SFML/Graphics.hpp>

#include "engine.hpp"
#include "timer.hpp"
#include "context.hpp"
#include "misc.hpp"

using namespace yae;

struct engine::impl {
    timer timer_absolute;
    timer timer_frame;
    rendering_context ctx;
    std::function<void(rendering_context&)> render_callback;
    std::function<void(rendering_context&, sf::Event&)> resize_callback;
    void run(sf::RenderWindow& window);
};

engine::engine()
    : pimpl(std::make_unique<impl>())
{
}

engine::engine(engine&& e)
{
    pimpl = std::move(e.pimpl);
}

engine::~engine()
{
}

void engine::run(sf::RenderWindow& window)
{
    pimpl->run(window);
}

void engine::set_render_callback(std::function<void(rendering_context&)> f)
{
    pimpl->render_callback = f;
}

void engine::set_resize_callback(std::function<void(rendering_context&, sf::Event&)> f)
{
    pimpl->resize_callback = f;
}

void engine::impl::run(sf::RenderWindow& window)
{
    while (true) {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count % 100] = timer_frame.elapsed();
        timer_frame.reset();
        check_for_opengl_errors();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return;
            }
            if (event.type == sf::Event::Resized) {
                resize_callback(ctx, event);
            }
            if (event.type == sf::Event::KeyPressed) {
                return;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_callback(ctx);
        window.display();
        ctx.frame_count++;
    }
}   
