
#include "sdl_backend.hpp"

yae::sdl_backend::sdl_backend()
{
    SDL_Init(SDL_INIT_EVERYTHING);
}

yae::sdl_backend::~sdl_backend()
{
    SDL_Quit();
}

struct sdl_window : yae::window {
    SDL_Window* w;
    SDL_GLContext ctx;
    sdl_window(SDL_Window* w, SDL_GLContext ctx);
    ~sdl_window();
};

sdl_window::sdl_window(SDL_Window* w, SDL_GLContext ctx)
    : w(w), ctx(ctx)
{
}

sdl_window::~sdl_window()
{
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(w); w = nullptr;
}

std::unique_ptr<sdl_window> create_simple_window()
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
