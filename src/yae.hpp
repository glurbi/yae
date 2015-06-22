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

struct event {
    event(int v) : value(v) {}
    int value;
};

class window {
public:
    virtual int width() = 0;
    virtual int height() = 0;
    virtual void swap() = 0;
    virtual std::vector<::yae::event> events() = 0;
};

struct yae {
    yae() {}
    virtual std::unique_ptr<window> create_simple_window() = 0;
    virtual int quit() = 0;
    virtual int keydown() = 0;
    virtual int window_resized() = 0;
};

}

#endif
