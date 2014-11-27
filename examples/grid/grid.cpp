#include <GL/glew.h>
#include "yae.hpp"

int main()
{
    auto yae = yae::yae{};
    auto engine = yae::engine{};
    auto grid = make_grid<float>(10,20);

    auto node = std::make_shared<geometry_node<float>>(std::move(grid));
    auto root = std::make_shared<group>();
    root->add(node);
    auto monochrome_program = monochrome_program::create();

    engine.set_callback([&](rendering_context& ctx) {
        engine.get_camera().render(root, ctx, monochrome_program);
    });

    engine.run();
}
