#ifndef _sdl_backend_hpp_
#define _sdl_backend_hpp_

#include <SDL.h>
#include <memory>
#include "yae.hpp"

namespace yae {

struct sdl_backend {

    sdl_backend();
    ~sdl_backend();

    static const int quit = SDL_QUIT;
    static const int keydown = SDL_KEYDOWN;
    static const int window_resized = SDL_WINDOWEVENT_RESIZED;

    static std::unique_ptr<::yae::window> create_simple_window();
};

}

#endif
