#ifndef _engine_hpp_
#define _engine_hpp_

#include <memory>
#include <functional>

#include "graph.hpp"

namespace yae {

class engine {

public:
    engine();
    ~engine();
    void run();
    void set_callback(std::function<void (rendering_context&)> f);
    camera& get_camera();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

}

#endif
