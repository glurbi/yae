#ifndef _engine_hpp_
#define _engine_hpp_

#include <memory>
#include <functional>

#include "yae.hpp"

namespace yae {

class window;
struct yae;

struct engine {
    engine();
    engine(engine&& e);
    ~engine();
    void run(window* win, ::yae::yae& yae);
    void set_render_callback(std::function<void(rendering_context&)> f);
    void set_resize_callback(std::function<void(rendering_context&)> f);
private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

}

#endif
