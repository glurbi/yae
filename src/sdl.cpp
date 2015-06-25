#include "sdl.hpp"

using namespace yae;

sdl_engine::sdl_engine()
{
    SDL_Init(SDL_INIT_EVERYTHING);
}

sdl_engine::~sdl_engine()
{
    SDL_Quit();
}

struct sdl_window : window {
    SDL_Window* win;
    SDL_GLContext ctx;
    sdl_window(SDL_Window* w, SDL_GLContext ctx);
    ~sdl_window();
    int width();
    int height();
    void swap();
    std::vector<::event> events();
    int quit() { return SDL_QUIT; }
    int keydown() { return SDL_KEYDOWN; }
    int window_resized() { return SDL_WINDOWEVENT_RESIZED; }
    void make_current();
};

sdl_window::sdl_window(SDL_Window* win, SDL_GLContext ctx)
    : win(win), ctx(ctx)
{
}

sdl_window::~sdl_window()
{
    SDL_GL_DeleteContext(ctx);
    if (win != nullptr) {
        SDL_DestroyWindow(win);
    }
    win = nullptr;
}

int sdl_window::width()
{
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    return w;
}

int sdl_window::height()
{
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    return h;
}

void sdl_window::swap()
{
    SDL_GL_SwapWindow(win);
}

std::vector<event> sdl_window::events()
{
    std::vector<event> v;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
        case SDL_KEYDOWN:
            v.push_back(e.type);
            break;
        case SDL_WINDOWEVENT:
            switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                v.push_back(e.window.event);
                break;
            }
            break;
        }
    }
    return v;
}

void sdl_window::make_current()
{
    SDL_GL_MakeCurrent(win, ctx);
}

std::unique_ptr<window> sdl_engine::create_simple_window()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_Window* win = SDL_CreateWindow("GLEW Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    glewInit();
    glViewport(0, 0, 800, 600);
    return std::make_unique<sdl_window>(win, ctx);
}

