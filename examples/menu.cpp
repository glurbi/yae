#include <iostream>
#include <memory>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <cstdlib>

#include "misc.hpp"
#include "timer.hpp"
#include "matrix.hpp"
#include "amazing.hpp"
#include "geometry.hpp"
#include "program.hpp"
#include "graph.hpp"

void draw_left_arrow(sf::RenderWindow& window, sf::Color& color) {
    window.pushGLStates();
    float ratio = 20.0f;
    float radius = window.getSize().x / ratio;
    sf::CircleShape left_circle = sf::CircleShape(radius);
    left_circle.setOutlineThickness(3.0f);
    left_circle.setOutlineColor(color);
    left_circle.setPosition(sf::Vector2f(radius / 2.0f + left_circle.getOutlineThickness(), window.getSize().y / 2.0f - radius));
    left_circle.setFillColor(sf::Color(255, 255, 255, 140));
    window.draw(left_circle);
    sf::ConvexShape left_arrow = sf::ConvexShape(3);
    left_arrow.setFillColor(color);
    left_arrow.setPosition(sf::Vector2f(radius / 2.0f + left_circle.getOutlineThickness() + radius, window.getSize().y / 2.0f));
    left_arrow.setPoint(0, sf::Vector2f(radius*.7f, -radius*.7f));
    left_arrow.setPoint(1, sf::Vector2f(radius*.7f, radius*.7f));
    left_arrow.setPoint(2, sf::Vector2f(-radius, 0));
    window.draw(left_arrow);
    window.popGLStates();
}

void draw_right_arrow(sf::RenderWindow& window, sf::Color& color) {
    window.pushGLStates();
    float ratio = 20.0f;
    float radius = window.getSize().x / ratio;
    sf::CircleShape right_circle = sf::CircleShape(radius);
    right_circle.setOutlineThickness(3.0f);
    right_circle.setOutlineColor(color);
    right_circle.setPosition(sf::Vector2f(window.getSize().x - radius*2.0f - right_circle.getOutlineThickness() - radius / 2.0f, window.getSize().y / 2.0f - radius));
    right_circle.setFillColor(sf::Color(255, 255, 255, 140));
    window.draw(right_circle);
    sf::ConvexShape right_arrow = sf::ConvexShape(3);
    right_arrow.setFillColor(color);
    right_arrow.setPosition(sf::Vector2f(window.getSize().x - radius - right_circle.getOutlineThickness() - radius / 2.0f, window.getSize().y / 2.0f));
    right_arrow.setPoint(0, sf::Vector2f(-radius*.7f, -radius*.7f));
    right_arrow.setPoint(1, sf::Vector2f(-radius*.7f, radius*.7f));
    right_arrow.setPoint(2, sf::Vector2f(radius, 0));
    window.draw(right_arrow);
    window.popGLStates();
}

static std::shared_ptr<camera> create_camera(maze_model& model, sf::RenderWindow& window) {
    float aspectRatio = (float)window.getSize().x / window.getSize().y;
    float mf = 0.5f; // margin factor, i.e. how much blank space around the maze
    clipping_volume cv;
    cv.left = -model.get_width() * mf;
    cv.right = model.get_width() * mf;
    cv.bottom = -model.get_width() / aspectRatio * mf;
    cv.top = model.get_width() / aspectRatio * mf;
    cv.nearp = 1.0f*(model.get_width() + model.get_height());
    cv.farp = 3.0f*(model.get_width() + model.get_height());
    auto camera =  std::make_shared<perspective_camera>(perspective_camera(cv));
    camera->move_backward(2.0f*(model.get_width()+model.get_height()));
    return camera;
}

menu_choice show_maze(sf::RenderWindow& window, maze_model& model, bool left_arrow_enabled,
    bool right_arrow_enabled, const color& col, sf::Font& font) {

    timer timer_absolute;
    timer timer_frame;

    maze_geometry_builder_3d builder3d(model);
    std::shared_ptr<geometry<float>> mazeGeom3d = builder3d.build();
    std::shared_ptr<geometry_node<float>> maze_node = std::make_shared<geometry_node<float>>(geometry_node<float>(mazeGeom3d));

    std::shared_ptr<camera> camera = create_camera(model, window);

    auto root = std::make_shared<group>(group());
    auto gr1 = std::make_shared<group>(group());
    auto gr2 = std::make_shared<group>(group());
    gr2->transformation(translation(-model.get_width() / 2.0f + 0.5f, -model.get_height() / 2.0f + 0.5f, 0.0f));
    gr2->add(maze_node);
    gr1->add(gr2);
    root->add(gr1);

    rendering_context ctx;
    ctx.dir = vector3(0, 0, -1.0f);
    ctx.frame_count = 0;

    std::shared_ptr<flat_shading_program> flat_shading_pr = flat_shading_program::Create();

    bool fullscreen = false;

    menu_choice choice = menu_choice::undefined;
    while (choice == menu_choice::undefined)
    {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count%100] = timer_frame.elapsed();
        timer_frame.reset();
        check_for_opengl_errors();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                choice = menu_choice::exit;
            }
            if (event.type == sf::Event::Resized) {
                camera = create_camera(model, window);
                glViewport(0, 0, event.size.width, event.size.height);
                sf::View view(sf::FloatRect(0, 0, (float) event.size.width, (float) event.size.height));
                window.setView(view);
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                    choice = menu_choice::exit;
                    break;
                case sf::Keyboard::Left:
                    choice = menu_choice::previous_maze;
                    break;
                case sf::Keyboard::Right:
                    choice = menu_choice::next_maze;
                    break;
                case sf::Keyboard::F11:
                    {
                        sf::ContextSettings settings;
                        settings.antialiasingLevel = 2;
                        settings.depthBits = 16;
                        if (fullscreen) {
                            window.create(sf::VideoMode(800, 600), "Amazing!", sf::Style::Default, settings);
                        } else {
                            window.create(sf::VideoMode::getFullscreenModes()[0], "Amazing!", sf::Style::Fullscreen, settings);
                        }
                        camera = create_camera(model, window);
                        int width = window.getSize().x;
                        int height = window.getSize().y;
                        glViewport(0, 0, width, height);
                        sf::View view(sf::FloatRect(0, 0, (float)width, (float)height));
                        window.setView(view);
                        fullscreen = !fullscreen;
                    }
                    window.setVerticalSyncEnabled(true);
                    break;
                case sf::Keyboard::Return:
                    play(model, window, color(col), font);
                    camera = create_camera(model, window);
                    int width = window.getSize().x;
                    int height = window.getSize().y;
                    glViewport(0, 0, width, height);
                    sf::View view(sf::FloatRect(0, 0, (float)width, (float)height));
                    window.setView(view);
                    break;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        //glFrontFace(GL_CCW);
        //glDisable(GL_BLEND);
        root->transformation(rotation((float)sin(ctx.elapsed_time_seconds / 2) * 180, 1.0f, 0.0f, 0.0f));
        gr1->transformation(rotation((float)sin(ctx.elapsed_time_seconds) * 180, 0.0f, 1.0f, 0.0f));
        flat_shading_pr->set_color(col);
        camera->render(root, ctx, flat_shading_pr);

        sf::Color arrow_colors[] = { sf::Color(128, 128, 128, 255), sf::Color(255, 255, 255, 255) };
        //draw_left_arrow(window, arrow_colors[left_arrow_enabled]);
        //draw_right_arrow(window, arrow_colors[right_arrow_enabled]);

        window.display();
        ctx.frame_count++;
    }

    return choice;
}

void menu(sf::RenderWindow& window, sf::Font& font) {
    int index = 0;
    const int len = 10;
    const int sizes[] = { 11, 17, 25, 31, 41, 51, 65, 87, 101, 123, 181 };
    const color colors[] = {
        color(0.0f, 1.0f, 0.0f),
        color(0.0f, 0.0f, 1.0f),
        color(1.0f, 0.0f, 0.0f),
        color(0.5f, 0.5f, 0.5f),
        color(0.0f, 0.7f, 0.9f),
        color(0.7f, 0.9f, 0.0f),
        color(0.7f, 0.5f, 0.3f),
        color(0.5f, 0.3f, 0.7f),
        color(0.3f, 0.7f, 0.5f),
        color(1.0f, 0.3f, 0.4f)
    };
    menu_choice choice = menu_choice::undefined;
    while (choice != menu_choice::exit) {
        const int mazeWidth = sizes[index];
        const int mazeHeight = sizes[index];
        maze_model model(mazeWidth, mazeHeight);
        model.create();
        maze_geometry_builder_3d builder3d(model);
        std::shared_ptr<geometry<float>> mazeGeom3d = builder3d.build();
        std::shared_ptr<geometry_node<float>> mazeNode = std::make_shared<geometry_node<float>>(geometry_node<float>(mazeGeom3d));
        choice = show_maze(window, model, index > 0, index < len - 1, colors[index], font);
        switch (choice) {
        case menu_choice::next_maze:
            if (index < len - 1) index++;
            break;
        case menu_choice::previous_maze:
            if (index > 0) index--;
            break;
        case menu_choice::select_maze:
            std::cout << "play with " << index << std::endl;
            break;
        case menu_choice::exit:
            exit(0);
        }
    }
}
