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

class window {
public:
    virtual int width() = 0;
    virtual int height() = 0;
    virtual void swap() = 0;
};

template<class T>
class yae {
public:

    yae() {}

    const int quit = T::quit;
    const int keydown = T::keydown;
    const int window_resized = T::window_resized;

    std::unique_ptr<window> create_simple_window() { return backend.create_simple_window(); }
private:
    T backend;
};

}

#endif
