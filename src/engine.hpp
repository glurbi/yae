#ifndef _engine_hpp_
#define _engine_hpp_

#include <memory>
#include <functional>

#include "graph.hpp"

namespace yae {

struct engine {
    engine();
    engine(engine&& e);
    ~engine();
    void run(SDL_Window* window);
    void set_render_callback(std::function<void(rendering_context&)> f);
    void set_resize_callback(std::function<void(rendering_context&, SDL_Event event)> f);
private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

}

#endif
