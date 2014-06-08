#include <iostream>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>

#include "misc.hpp"
#include "timer.hpp"
#include "matrix.hpp"
#include "geometry.hpp"
#include "graph.hpp"
#include "program.hpp"
#include "amazing.hpp"

int main() {
    srand((unsigned int)time(0));
    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    settings.depthBits = 16;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Amazing!", sf::Style::Default, settings);
    //sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Amazing!", sf::Style::Fullscreen, settings);
    //window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);
    sf::Font font;
    if (!font.loadFromFile("anonymous.ttf")) {
        return -1;
    }
    glewInit();
    glViewport(0, 0, window.getSize().x, window.getSize().y);
    menu(window, font);
}
