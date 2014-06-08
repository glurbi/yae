#ifndef _amazing_hpp_
#define _amazing_hpp_

#include <SFML/Graphics.hpp>

#include <vector>
#include <set>
#include <memory>

#include "geometry.hpp"
#include "context.hpp"

struct cell {
    int x;
    int y;
    bool wall;
    struct comp	{
        bool operator() (const cell& c1, const cell& c2) const;
    };
};

struct pos {
    int x;
    int y;
    inline bool operator==(const pos& p) { return x == p.x && y == p.y; }
};

class maze_model {
public:
    maze_model(int width_, int height_);
    void create();
    int get_width();
    int get_height();
    inline cell& get_cell(int x, int y) { return cells[x + y*width]; }
    inline cell& get_cell(pos p) { return get_cell(p.x, p.y); }
    inline bool is_wall(int x, int y) { return (x < 0) || (x >= width) || (y < 0) || (y >= height) || get_cell(x, y).wall; }
    inline bool is_wall(float x, float y) { return is_wall((int)floor(x), (int)floor(y)); }
    inline std::vector<cell>& get_cells() { return cells; }
    inline bool is_like_wall(int x, int y) { return (x < 0) || (x >= width) || (y < 0) || (y >= height) || get_cell(x, y).wall; }
    inline bool contains(cell& c, std::set<cell, cell::comp>& visited) { return visited.find(c) != visited.end(); }
    pos find_empty_cell(int line, int col);
private:
    void visit(cell& c, std::set<cell, cell::comp>& visited, int& count);
    int width;
    int height;
    std::vector<cell> cells;
    unsigned seed;
};

class maze_geometry_builder_2d {
public:
    maze_geometry_builder_2d(maze_model& model_);
    std::shared_ptr<geometry<float>> build();
private:
    maze_model& model;
};

class maze_geometry_builder_3d {
public:
    maze_geometry_builder_3d(maze_model& model_);
    std::shared_ptr<geometry<float>> build();
private:
    maze_model& model;
};

class hero_builder_2d {
public:
    hero_builder_2d();
    std::shared_ptr<geometry<float>> build();
};

class multi_hero_builder_2d {
public:
    multi_hero_builder_2d();
    std::shared_ptr<geometry<float>> build();
};

class bad_guy_builder_2d {
public:
    bad_guy_builder_2d();
    std::shared_ptr<geometry<float>> build();
};

enum class menu_choice {
    undefined,
    select_maze,
    next_maze,
    previous_maze,
    exit
};

void menu(sf::RenderWindow& window, sf::Font& font);

void play(maze_model& model, sf::RenderWindow& window, color color, sf::Font& font);

void ending(sf::RenderWindow& window, sf::Font& font, std::string text, std::shared_ptr<texture> tex);

#endif
