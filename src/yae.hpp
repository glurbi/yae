#ifndef _yae_hpp_
#define _yae_hpp_

#include <memory>

// otherwise SDL redefines main()
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <SDL_image.h>

#include "misc.hpp"
#include "timer.hpp"
#include "matrix.hpp"
#include "graph.hpp"
#include "program.hpp"
#include "geometry.hpp"
#include "timer.hpp"
#include "context.hpp"
#include "engine.hpp"

namespace yae {

class window;

template<class T>
class yae {
public:

    yae();

    static const int quit = T::quit;
    static const int keydown = T::keydown;
    static const int window_resized = T::window_resized;

    static std::unique_ptr<window> create_simple_window();
};

class window {

};

}

#include "yae_impl.hpp"

#endif
