#include <GL/glew.h>
#include <SDL.h>

#include "engine.hpp"
#include "timer.hpp"
#include "context.hpp"
#include "misc.hpp"
#include "yae.hpp"

using namespace yae;

struct engine::impl {
    timer timer_absolute;
    timer timer_frame;
    rendering_context ctx;
    std::function<void(rendering_context&)> render_callback;
    std::function<void(rendering_context&)> resize_callback;
    void run(window* win, ::yae::yae& yae);
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

void engine::run(window* win, ::yae::yae& yae)
{
    pimpl->run(win, yae);
}

void engine::set_render_callback(std::function<void(rendering_context&)> f)
{
    pimpl->render_callback = f;
}

void engine::set_resize_callback(std::function<void(rendering_context&)> f)
{
    pimpl->resize_callback = f;
}

void engine::impl::run(window* win, ::yae::yae& yae)
{
    while (true) {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count % 100] = timer_frame.elapsed();
        timer_frame.reset();
        check_for_opengl_errors();
        std::vector<::yae::event> events = win->events();
        for (::yae::event e : events) {
            if (e.value == yae.quit() || e.value == yae.keydown()) {
                return;
            } else if (e.value == yae.window_resized()) {
                resize_callback(ctx);
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_callback(ctx);
        win->swap();
        ctx.frame_count++;
    }
}   
