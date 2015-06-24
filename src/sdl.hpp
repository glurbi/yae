#ifndef _sdl_hpp_
#define _sdl_hpp_

#include <memory>

#define SDL_MAIN_HANDLED // otherwise SDL redefines main()
#include <SDL.h>
#include <SDL_image.h>

#include "yae.hpp"

namespace yae {

struct sdl_engine : yae::engine {

    sdl_engine();
    ~sdl_engine();

    std::unique_ptr<window> create_simple_window();
};

}

#endif
