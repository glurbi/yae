#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <random>
#include <stdexcept>

#include "amazing.hpp"

bool cell::comp::operator() (const cell& c1, const cell& c2) const {
	if (c1.x < c2.x) {
		return true;
	} else if (c1.x == c2.x)
		return c1.y < c2.y;
	else {
		return false;
	}
}

maze_model::maze_model(int width_, int height_) :
    width(width_), height(height_), cells(width*height, cell())
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			get_cell(i,j).x = i;
			get_cell(i,j).y = j;
			get_cell(i,j).wall = true;
		}
	}
}

void maze_model::create() {
    std::set<cell, cell::comp> visited;
    int count = 0;
    visit(get_cell(1,1), visited, count);
    get_cell(0, 1).wall = false;
    get_cell(width-1, height-2).wall = false;
}

int maze_model::get_width() {
    return width;
}

int maze_model::get_height() {
    return height;
}

pos maze_model::find_empty_cell(int line, int col) {
    for (int y = line - 1; y <= line + 1; y++) {
        for (int x = col - 1; x <= col + 1; x++) {
            if (!get_cell(col, y).wall) {
                return pos{ col, y };
            }
        }
    }
    throw std::exception();
}

void maze_model::visit(cell& c, std::set<cell, cell::comp>& visited, int& count) {
	c.wall = false;
    visited.insert(c);
    std::vector<cell*> neighbors;
	if ((c.x > 2) && !contains(get_cell(c.x-2, c.y), visited)) neighbors.push_back(&get_cell(c.x-2, c.y));
	if ((c.x < width-2) && !contains(get_cell(c.x+2, c.y), visited)) neighbors.push_back(&get_cell(c.x+2, c.y));
	if ((c.y > 2) && !contains(get_cell(c.x, c.y-2), visited)) neighbors.push_back(&get_cell(c.x, c.y-2));
	if ((c.y < height-2) && !contains(get_cell(c.x, c.y+2), visited)) neighbors.push_back(&get_cell(c.x, c.y+2));
	std::random_shuffle(neighbors.begin(), neighbors.end());
    for (auto& n : neighbors) {
		if ((visited.find(*n) != visited.end()) && (count % 11 != 0)) {
            continue;
        }
		int wx = (n->x + c.x) / 2;
		int wy = (n->y + c.y) / 2;
		get_cell(wx, wy).wall = false;
        visit(*n, visited, count);
        count++;
    }
}

maze_geometry_builder_2d ::maze_geometry_builder_2d(maze_model& model_) : model(model_) {}

std::shared_ptr<geometry<float>> maze_geometry_builder_2d::build() {
    std::vector<float> v;
    for (auto& cell : model.get_cells()) {
        if (cell.wall) {
            v.push_back(cell.x+0.0f); v.push_back(cell.y+0.0f);
            v.push_back(cell.x+1.0f); v.push_back(cell.y+0.0f);
            v.push_back(cell.x+1.0f); v.push_back(cell.y+1.0f);
            v.push_back(cell.x+0.0f); v.push_back(cell.y+1.0f);
        }
    }
    auto mazeGeom = std::make_shared<geometry<float>>(geometry<float>(v.size()/2));
    mazeGeom->set_vertex_positions(&v[0], v.size() * sizeof(float));
    return mazeGeom;
}

maze_geometry_builder_3d ::maze_geometry_builder_3d(maze_model& model_) : model(model_) {}

std::shared_ptr<geometry<float>> maze_geometry_builder_3d::build() {
    buffer_object_builder<float> posb;
    buffer_object_builder<float> norb;
    for (auto& cell : model.get_cells()) {
        if (cell.wall) {
            // top
            posb << cell.x + 0.0f << cell.y + 0.0f << 1.0f;
            posb << cell.x + 1.0f << cell.y + 0.0f << 1.0f;
            posb << cell.x + 1.0f << cell.y + 1.0f << 1.0f;
            posb << cell.x + 0.0f << cell.y + 1.0f << 1.0f;
            for (int i = 0; i < 4; i++) norb << 0.0f << 0.0f << 1.0f;
            // bottom
            posb << cell.x + 0.0f << cell.y + 0.0f << 0.0f;
            posb << cell.x + 0.0f << cell.y + 1.0f << 0.0f;
            posb << cell.x + 1.0f << cell.y + 1.0f << 0.0f;
            posb << cell.x + 1.0f << cell.y + 0.0f << 0.0f;
            for (int i = 0; i < 4; i++) norb << 0.0f << 0.0f << -1.0f;
            // right
            posb << cell.x + 1.0f << cell.y + 0.0f << 1.0f;
            posb << cell.x + 1.0f << cell.y + 0.0f << 0.0f;
            posb << cell.x + 1.0f << cell.y + 1.0f << 0.0f;
            posb << cell.x + 1.0f << cell.y + 1.0f << 1.0f;
            for (int i = 0; i < 4; i++) norb << 1.0f << 0.0f << 0.0f;
            // left
            posb << cell.x + 0.0f << cell.y + 0.0f << 1.0f;
            posb << cell.x + 0.0f << cell.y + 1.0f << 1.0f;
            posb << cell.x + 0.0f << cell.y + 1.0f << 0.0f;
            posb << cell.x + 0.0f << cell.y + 0.0f << 0.0f;
            for (int i = 0; i < 4; i++) norb << -1.0f << 0.0f << 0.0f;
            // front
            posb << cell.x + 0.0f << cell.y + 0.0f << 1.0f;
            posb << cell.x + 0.0f << cell.y + 0.0f << 0.0f;
            posb << cell.x + 1.0f << cell.y + 0.0f << 0.0f;
            posb << cell.x + 1.0f << cell.y + 0.0f << 1.0f;
            for (int i = 0; i < 4; i++) norb << 0.0f << -1.0f << 0.0f;
            // back
            posb << cell.x + 0.0f << cell.y + 1.0f << 1.0f;
            posb << cell.x + 1.0f << cell.y + 1.0f << 1.0f;
            posb << cell.x + 1.0f << cell.y + 1.0f << 0.0f;
            posb << cell.x + 0.0f << cell.y + 1.0f << 0.0f;
            for (int i = 0; i < 4; i++) norb << 0.0f << 1.0f << 0.0f;
        }
    }
    auto mazeGeom = std::make_shared<geometry<float>>(geometry<float>(posb.get_count()/3));
    mazeGeom->set_vertex_positions(posb.build());
    mazeGeom->set_vertex_normals(norb.build());
    return mazeGeom;
}

hero_builder_2d::hero_builder_2d() {}

std::shared_ptr<geometry<float>> hero_builder_2d::build() {
    buffer_object_builder<float> b;
    b << 0.0f << 0.0f;
    b << 1.0f << 0.0f;
    b << 1.0f << 1.0f;
    b << 0.0f << 1.0f;
    auto hero = std::make_shared<geometry<float>>(geometry<float>(b.get_size() / 2));
    hero->set_vertex_positions(b.build());
    hero->set_vertex_tex_coords(b.build());
    return hero;
}

multi_hero_builder_2d::multi_hero_builder_2d() {}

std::shared_ptr<geometry<float>> multi_hero_builder_2d::build() {
    buffer_object_builder<float> b;
    b << -50.0f << -50.0f;
    b << 50.0f << -50.0f;
    b << 50.0f << 50.0f;
    b << -50.0f << 50.0f;
    auto multi_hero = std::make_shared<geometry<float>>(geometry<float>(b.get_size() / 2));
    multi_hero->set_vertex_positions(b.build());
    multi_hero->set_vertex_tex_coords(b.build());
    return multi_hero;
}

bad_guy_builder_2d::bad_guy_builder_2d() {}

std::shared_ptr<geometry<float>> bad_guy_builder_2d::build() {
    buffer_object_builder<float> b;
    b << 0.0f << 0.0f;
    b << 1.0f << 0.0f;
    b << 1.0f << 1.0f;
    b << 0.0f << 1.0f;
    auto bad_guy = std::make_shared<geometry<float>>(geometry<float>(b.get_size() / 2));
    bad_guy->set_vertex_positions(b.build());
    bad_guy->set_vertex_tex_coords(b.build());
    return bad_guy;
}
