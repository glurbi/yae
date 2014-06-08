#include <iostream>
#include <numeric>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <map>
#include <array>

#include "amazing.hpp"
#include "timer.hpp"
#include "graph.hpp"
#include "geometry.hpp"
#include "misc.hpp"
#include "texture.hpp"

typedef int distance;

enum class direction {
    none, up, down, right, left
};

enum class actor_nature {
    good, evil
};

struct movement {
    int dx;
    int dy;
};

std::map<direction, movement> move = {
    { direction::up, { 0, 1 } },
    { direction::down, { 0, -1 } },
    { direction::right, { 1, 0 } },
    { direction::left, { -1, 0 } }
};

struct actor_data {
    std::shared_ptr<group> actor_group;
    int pos_x;
    int pos_y;
    float pos_fx;
    float pos_fy;
    direction dir;
    direction next_direction;
    float inc;
    actor_nature nature;
};

struct game_data {
    game_data(maze_model& model) : model(model) {}
    maze_model& model;
    std::shared_ptr<camera> cam;
    std::shared_ptr<actor_data> hero_data;
    std::vector<std::shared_ptr<actor_data>> bad_guys_data;
};

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

bool is_int(float f, float eps) {
    return fabs(f - round(f)) < eps;
}

void update_position(actor_data& ad, maze_model& model, rendering_context& ctx) {
    switch (ad.dir) {
    case direction::left:
        if (!model.is_wall(ad.pos_x - 1, ad.pos_y)) ad.pos_fx -= ad.inc;
        break;
    case direction::right:
        if (!model.is_wall(ad.pos_x + 1, ad.pos_y)) { ad.pos_fx += ad.inc;  }
        break;
    case direction::up:
        if (!model.is_wall(ad.pos_x, ad.pos_y + 1)) ad.pos_fy += ad.inc;
        break;
    case direction::down:
        if (!model.is_wall(ad.pos_x, ad.pos_y - 1)) ad.pos_fy -= ad.inc;
        break;
    default:
        break;
    };
    if (is_int(ad.pos_fx, ad.inc / 10.0f) && (is_int(ad.pos_fy, ad.inc / 10.0f))) {
        ad.pos_x = (int)round(ad.pos_fx);
        ad.pos_y = (int)round(ad.pos_fy);
        ad.pos_fx = (float)ad.pos_x;
        ad.pos_fy = (float)ad.pos_y;
        ad.dir = ad.next_direction;
    }
    ad.actor_group->transformation(translation(ad.pos_fx, ad.pos_fy, 0.0f));
}

std::shared_ptr<game_data> make_game_data(maze_model& model, sf::RenderWindow& window) {
    auto game = std::make_shared<game_data>(game_data(model));
    game->cam = create_camera(window);
    game->cam->move_up(1.5f);
    game->cam->move_right(0.5f);
    std::shared_ptr<actor_data> hero_data = std::make_shared<actor_data>(actor_data());
    hero_data->pos_x = 0;
    hero_data->pos_y = 1;
    hero_data->pos_fx = 0;
    hero_data->pos_fy = 1;
    hero_data->dir = direction::none;
    hero_data->next_direction = direction::none;
    hero_data->inc = 0.1f;
    hero_data->nature = actor_nature::good;
    hero_builder_2d hero_builder;
    auto hero = hero_builder.build();
    auto hero_node = std::make_shared<geometry_node<float>>(geometry_node<float>(hero));
    auto hero_group = std::make_shared<group>(group());
    hero_group->add(hero_node);
    hero_data->actor_group = hero_group;
    game->hero_data = hero_data;
    for (int i = 0; i < model.get_height() / 10; i++) {
        std::shared_ptr<actor_data> bad_guy_data = std::make_shared<actor_data>(actor_data());
        pos p = model.find_empty_cell(model.get_height() - 2 - i * 10, model.get_width() - 2 - i * 10);
        bad_guy_data->pos_x = p.x;
        bad_guy_data->pos_y = p.y;
        bad_guy_data->pos_fx = (float) p.x;
        bad_guy_data->pos_fy = (float) p.y;
        bad_guy_data->dir = direction::none;
        bad_guy_data->next_direction = direction::none;
        bad_guy_data->inc = 0.05f;
        bad_guy_data->nature = actor_nature::evil;
        bad_guy_builder_2d bad_guy_builder;
        auto bad_guy = bad_guy_builder.build();
        auto bad_guy_node = std::make_shared<geometry_node<float>>(geometry_node<float>(bad_guy));
        auto bad_guy_group = std::make_shared<group>(group());
        bad_guy_group->add(bad_guy_node);
        bad_guy_data->actor_group = bad_guy_group;
        game->bad_guys_data.push_back(bad_guy_data);
    }
    return game;
}

std::shared_ptr<texture> make_hero_texture() {
    sf::Image hero_image;
    if (!hero_image.loadFromFile("smiley.png")) {
        std::cout << "Failed to load smiley.png" << std::endl;
    }
    hero_image.flipVertically();
    return std::make_shared<texture>((GLubyte*)hero_image.getPixelsPtr(), hero_image.getSize().x, hero_image.getSize().y);
}

std::shared_ptr<texture> make_bad_guy_texture() {
    sf::Image bad_guy_image;
    if (!bad_guy_image.loadFromFile("evil.png")) {
        std::cout << "Failed to load evil.png" << std::endl;
    }
    bad_guy_image.flipVertically();
    return std::make_shared<texture>((GLubyte*)bad_guy_image.getPixelsPtr(), bad_guy_image.getSize().x, bad_guy_image.getSize().y);
}

std::shared_ptr<rendering_context> make_rendering_context() {
    std::shared_ptr<rendering_context> ctx = std::make_shared<rendering_context>();
    ctx->frame_count = 0;
    return ctx;
}

std::shared_ptr<group> make_maze_group(maze_model& model) {
    auto maze_group = std::make_shared<group>(group());
    maze_geometry_builder_2d builder2d(model);
    auto maze_geom_2d = builder2d.build();
    auto maze_node = std::make_shared<geometry_node<float>>(geometry_node<float>(maze_geom_2d));
    maze_group->add(maze_node);
    return maze_group;
}

int handle_events(sf::RenderWindow& window, std::shared_ptr<game_data> game) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            return -1;
        }
        if (event.type == sf::Event::Resized) {
            game->cam = create_camera(window);
            glViewport(0, 0, event.size.width, event.size.height);
            sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
            window.setView(view);
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Escape:
                return -1;
                break;
            case sf::Keyboard::Left:
                game->hero_data->next_direction = direction::left;
                break;
            case sf::Keyboard::Right:
                game->hero_data->next_direction = direction::right;
                break;
            case sf::Keyboard::Up:
                game->hero_data->next_direction = direction::up;
                break;
            case sf::Keyboard::Down:
                game->hero_data->next_direction = direction::down;
                break;
            }
        }
    }
    return 0;
}

class mat {
public:
    mat(int w, int h) : w(w), h(h) {
        ia = new int[w*h];
        for (int i = 0; i < w*h; ++i) {
            ia[i] = std::numeric_limits<int>::max();
        }
    }
    ~mat() { delete[] ia; }
    int& elem(int x, int y) { return ia[w*y + x]; }
private:
    int w;
    int h;
    int* ia;
};

distance get_shortest_distance(pos src, pos dest, direction dir, game_data& g, distance d, mat& m, int& best) {
    d++;
    if (d > best) return std::numeric_limits<int>::max();
    auto& movement = move[dir];
    src.x += movement.dx;
    src.y += movement.dy;
    if (g.model.is_like_wall(src.x, src.y)) return std::numeric_limits<int>::max();
    int& best_m = m.elem(src.x, src.y);
    if (d > best_m) return std::numeric_limits<int>::max();
    best_m = d;
    if (g.hero_data->pos_x == src.x && g.hero_data->pos_y == src.y) {
        best = d;
        return d;
    } else {
        for (auto dir : { direction::up, direction::down, direction::left, direction::right }) {
            get_shortest_distance(src, dest, dir, g, d, m, best);
        }
        return best;
    }
}

direction get_best_direction(pos& src, pos& dest, game_data& game, int best) {
    distance shortest = std::numeric_limits<int>::max();
    direction best_dir = direction::none;
    if (src == dest) return direction::none;
    mat m{ game.model.get_width(), game.model.get_height() };
    m.elem(src.x, src.y) = 0;
    int best_found = best;
    for (auto dir : { direction::up, direction::down, direction::left, direction::right }) {
        int d = get_shortest_distance(src, dest, dir, game, 0, m, best_found);
        if (d < shortest) {
            shortest = d;
            best_dir = dir;
        }
    }
    if (best == best_found) {
        best_dir = direction::none;
    }
    return best_dir;
}

void update_bad_guys_directions(game_data& game, rendering_context& ctx) {
    for (auto& bad_guy_data : game.bad_guys_data) {
        int best = 50;
        pos src{ int(bad_guy_data->pos_fx + 0.5), int(bad_guy_data->pos_fy + 0.5) };
        pos dest{ game.hero_data->pos_x, game.hero_data->pos_y };
        if (abs(src.x - dest.x) + abs(src.y - dest.y) < best) {
            bad_guy_data->next_direction = get_best_direction(src, dest, game, best);
        }
        if (bad_guy_data->next_direction == direction::none) {
            std::vector<direction> dirs = { direction::up, direction::down, direction::left, direction::right };
            bad_guy_data->next_direction = dirs[std::rand() % 4];
        }
    }
}

bool is_ending(maze_model& model, game_data* game, sf::RenderWindow& window, color color,
    sf::Font& font, std::shared_ptr<texture> hero_texture, std::shared_ptr<texture> bad_guy_texture)
{
    if (game->hero_data->pos_x == model.get_width() - 1 && game->hero_data->pos_y == model.get_height() - 2) {
        ending(window, font, "You win!", hero_texture);
        return true;
    }
    for (auto& bad_guy_data : game->bad_guys_data) {
        if (game->hero_data->pos_x == bad_guy_data->pos_x && game->hero_data->pos_y == bad_guy_data->pos_y) {
            ending(window, font, "You lose!", bad_guy_texture);
            return true;
        }
    }
    return false;
}

void play(maze_model& model, sf::RenderWindow& window, color color, sf::Font& font) {

    timer timer_absolute;
    timer timer_frame;

    std::shared_ptr<monochrome_program> monochrome_pr = monochrome_program::create();
    monochrome_pr->set_color(color);
    std::shared_ptr<texture_program> texture_pr = texture_program::create();

    auto game = make_game_data(model, window);
    auto maze_group = make_maze_group(model);
    auto ctx = make_rendering_context();
    auto hero_texture = make_hero_texture();
    auto bad_guy_texture = make_bad_guy_texture();

    while (true)
    {
        ctx->elapsed_time_seconds = timer_absolute.elapsed();
        ctx->last_frame_times_seconds[ctx->frame_count%100] = timer_frame.elapsed();
        double avg = std::accumulate(ctx->last_frame_times_seconds, ctx->last_frame_times_seconds + 100, 0.0) / 100.0;
        if (avg < 0.01) {
            long usec = (long) ((0.01-avg)*1000000);
            //std::this_thread::sleep_for(std::chrono::microseconds(usec));
            //std::cout << "sleeping for " << usec << std::endl;
        }
        timer_frame.reset();
        check_for_opengl_errors();
        if (handle_events(window, game) == -1) return;
        update_position(*game->hero_data, model, *ctx);
        for (auto& bad_guy_data : game->bad_guys_data) {
            update_position(*bad_guy_data, model, *ctx);
        }
        update_bad_guys_directions(*game, *ctx);
        game->cam->position_v = vector3(game->hero_data->pos_fx, game->hero_data->pos_fy, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        game->cam->render(maze_group, *ctx, monochrome_pr);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        texture_pr->set_texture(hero_texture);
        game->cam->render(game->hero_data->actor_group, *ctx, texture_pr);
        texture_pr->set_texture(bad_guy_texture);
        for (auto& bad_guy_data : game->bad_guys_data) {
            game->cam->render(bad_guy_data->actor_group, *ctx, texture_pr);
        }
        glDisable(GL_BLEND);
        window.display();

        ctx->frame_count++;
        if (is_ending(model, game.get(), window, color, font, hero_texture, bad_guy_texture)) break;
    }
}


