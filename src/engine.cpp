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
    std::function<void(rendering_context&, SDL_Event)> resize_callback;
    void run(Wwindow* win);
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

void engine::run(Wwindow* win)
{
    pimpl->run(win);
}

void engine::set_render_callback(std::function<void(rendering_context&)> f)
{
    pimpl->render_callback = f;
}

void engine::set_resize_callback(std::function<void(rendering_context&, SDL_Event event)> f)
{
    pimpl->resize_callback = f;
}

void engine::impl::run(Wwindow* win)
{
    while (true) {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count % 100] = timer_frame.elapsed();
        timer_frame.reset();
        check_for_opengl_errors();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
            case SDL_KEYDOWN:
                return;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    resize_callback(ctx, event);
                    break;
                }
                break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_callback(ctx);
        win->swap();
        ctx.frame_count++;
    }
}   
