#ifndef _sdl_backend_hpp_
#define _sdl_backend_hpp_

#include <SDL.h>
#include <memory>
#include "yae.hpp"

namespace yae {

struct sdl_backend : ::yae::yae {

    sdl_backend();
    ~sdl_backend();

    int quit() { return SDL_QUIT; }
    int keydown() { return SDL_KEYDOWN; }
    int window_resized() { return SDL_WINDOWEVENT_RESIZED; }

    std::unique_ptr<::yae::window> create_simple_window();
};

}

#endif
