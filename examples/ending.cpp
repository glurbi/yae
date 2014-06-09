#include <memory>
#include <GL/glew.h>
#include <iostream>
#include <numeric>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <stdlib.h>

#include "graph.hpp"
#include "geometry.hpp"
#include "amazing.hpp"
#include "timer.hpp"
#include "misc.hpp"

static std::shared_ptr<camera> create_camera(sf::RenderWindow& window) {
    clipping_volume cv;
    int div = 100;
    cv.right = (float)window.getSize().x / div;
    cv.left = (float)-(int)window.getSize().x / div;
    cv.bottom = (float)-(int)window.getSize().y / div;
    cv.top = (float)window.getSize().y / div;
    cv.nearp = 1.0f;
    cv.farp = -1.0f;
    return std::make_shared<parallel_camera>(parallel_camera(cv));
}

void ending(sf::RenderWindow& window, sf::Font& font, std::string text, std::shared_ptr<texture> tex) {

    timer timer_absolute;
    timer timer_frame;

    sf::Image heroImage;
    if (!heroImage.loadFromFile("smiley.png")) {
        return;
    }


    heroImage.flipVertically();
    auto heroTexture = std::make_shared<texture>((GLubyte*)heroImage.getPixelsPtr(), heroImage.getSize().x, heroImage.getSize().y);

    multi_hero_builder_2d builder;
    std::shared_ptr<geometry<float>> multi_hero = builder.build();
    std::shared_ptr<geometry_node<float>> node = std::make_shared<geometry_node<float>>(geometry_node<float>(multi_hero));

    auto camera = create_camera(window);
    auto root = std::make_shared<group>(group());
    root->add(node);
    rendering_context ctx;
    std::shared_ptr<texture_program> textureProgram = texture_program::create();
    textureProgram->set_texture(tex);
    ctx.frame_count = 0;

    sf::Text text1;
    text1.setFont(font);
    text1.setString(text);
    text1.setCharacterSize(142);
    text1.setColor(sf::Color::White);
    text1.setStyle(sf::Text::Regular);

    sf::Text text2;
    text2.setFont(font);
    text2.setString(text);
    text2.setCharacterSize(138);
    text2.setColor(sf::Color::Black);
    text2.setStyle(sf::Text::Bold);

    sf::FloatRect textRect1 = text1.getLocalBounds();
    text1.setOrigin(textRect1.left + textRect1.width / 2.0f, textRect1.top + textRect1.height / 2.0f);
    text1.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));

    sf::FloatRect textRect2 = text2.getLocalBounds();
    text2.setOrigin(textRect2.left + textRect2.width / 2.0f, textRect2.top + textRect2.height / 2.0f);
    text2.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));

    while (true)
    {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count % 100] = timer_frame.elapsed();
        double avg = std::accumulate(ctx.last_frame_times_seconds, ctx.last_frame_times_seconds + 100, 0.0) / 100.0;
        if (avg < 0.01) {
            long usec = (long)((0.01 - avg) * 1000000);
            std::this_thread::sleep_for(std::chrono::microseconds(usec));
            //std::cout << "sleeping for " << usec << std::endl;
        }
        timer_frame.reset();
        check_for_opengl_errors();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return;
            }
            if (event.type == sf::Event::Resized) {
                camera = create_camera(window);
                glViewport(0, 0, event.size.width, event.size.height);
                sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
                window.setView(view);
            }
            if (event.type == sf::Event::KeyPressed) {
                return;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera->render(root, ctx, textureProgram);
        window.pushGLStates();
        window.draw(text1);
        window.draw(text2);
        window.popGLStates();
        window.display();
        ctx.frame_count++;
    }

}
