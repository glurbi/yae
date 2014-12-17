#ifndef _misc_hpp_
#define _misc_hpp_

#include <memory>
#include <SFML/Graphics.hpp>

void check_for_opengl_errors();

std::unique_ptr<sf::RenderWindow> create_simple_window();

#endif