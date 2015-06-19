#ifndef _misc_hpp_
#define _misc_hpp_

#include <memory>
#include <SDL.h>

namespace yae {

void check_for_opengl_errors();

SDL_Window* create_simple_window();

}

#endif