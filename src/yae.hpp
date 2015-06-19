#ifndef _yae_hpp_
#define _yae_hpp_

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

class yae {
public:
    yae();
};

}

#endif
