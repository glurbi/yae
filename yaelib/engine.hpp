#ifndef _engine_hpp_
#define _engine_hpp_

#include <memory>
#include <functional>
#include <SFML/Graphics.hpp>

#include "graph.hpp"

namespace yae {

class engine {

public:
    engine();
    ~engine();
    void run();
    void set_render_callback(std::function<void(rendering_context&)> f);
    void set_resize_callback(std::function<void(rendering_context&, sf::Event&)> f);
    sf::RenderWindow& get_window();
    void set_window(sf::RenderWindow*);

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

}

#endif
