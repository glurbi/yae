#include <iostream>
#include <memory>
#include <iostream>
#include <numeric>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <vector>
#include <set>
#include <memory>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>

#include "misc.hpp"
#include "timer.hpp"
#include "matrix.hpp"
#include "graph.hpp"
#include "program.hpp"
#include "geometry.hpp"
#include "timer.hpp"
#include "context.hpp"

std::shared_ptr<camera> create_camera(sf::RenderWindow& window) {
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

int main() {

    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    settings.depthBits = 16;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Texture", sf::Style::Default, settings);
    //sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Amazing!", sf::Style::Fullscreen, settings);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);
    glewInit();
    glViewport(0, 0, window.getSize().x, window.getSize().y);

    timer timer_absolute;
    timer timer_frame;

    sf::Image heroImage;
    heroImage.loadFromFile("smiley.png");

    heroImage.flipVertically();
    auto heroTexture = std::make_shared<texture>((GLubyte*)heroImage.getPixelsPtr(), heroImage.getSize().x, heroImage.getSize().y);

    buffer_object_builder<float> b;
    b << -50.0f << -50.0f;
    b << 50.0f << -50.0f;
    b << 50.0f << 50.0f;
    b << -50.0f << 50.0f;
    auto multi_hero = std::make_shared<geometry<float>>(geometry<float>(b.get_size() / 2));
    multi_hero->set_vertex_positions(b.build());
    multi_hero->set_vertex_tex_coords(b.build());

    std::shared_ptr<geometry_node<float>> node = std::make_shared<geometry_node<float>>(geometry_node<float>(multi_hero));

    auto camera = create_camera(window);
    float cam_height = camera->get_height();
    float cam_width = camera->get_width();
    auto root = std::make_shared<group>(group());
    root->add(node);
    rendering_context ctx;
    std::shared_ptr<texture_program> textureProgram = texture_program::create();
    textureProgram->set_texture(heroTexture);
    ctx.frame_count = 0;

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
                return 0;
            }
            if (event.type == sf::Event::Resized) {
                camera = create_camera(window);
                glViewport(0, 0, event.size.width, event.size.height);
                sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
                window.setView(view);
            }
            if (event.type == sf::Event::KeyPressed) {
                return 0;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera->rotate_z(1);
        float f = (float)(1.0+0.5*sin(2.0*3.1415927*ctx.elapsed_time_seconds));
        camera->set_opening(cam_width * f, cam_height * f);
        camera->render(root, ctx, textureProgram);
        window.display();
        ctx.frame_count++;
    }

}
