#include <GL/glew.h>
#include "yae.hpp"

using namespace yae;

std::unique_ptr<camera> create_camera(sf::RenderWindow& window)
{
    clipping_volume cv;
    float ar = (float)window.getSize().x / window.getSize().y;
    cv.right = 2.0f;
    cv.left = -2.0f;
    cv.bottom = -2.0f / ar;
    cv.top = 2.0f / ar;
    cv.nearp = 2.0f;
    cv.farp = 100.0f;
    std::unique_ptr<camera> camera = std::make_unique<perspective_camera>(cv);
    camera->move_backward(20.0f);
    return camera;
}

int main()
{
    auto yae = ::yae::yae{};
    auto engine = ::yae::engine{};
    auto window = create_simple_window();
    auto camera = create_camera(*window);

    auto box = make_box<float>(10,20,5);
    auto node = std::make_shared<geometry_node<float>>(std::move(box));
    auto root = std::make_shared<group>();
    root->set_transform_callback([](rendering_context& ctx)
    {
        float f = (float)ctx.elapsed_time_seconds;
        return multm(rotation(10.0f*f, 1.0f, 0.0f, 0.0f),
                     rotation(20.0f*f, 0.0f, 1.0f, 0.0f),
                     rotation(50.0f*f, 0.0f, 0.0f, 1.0f));
    });
    root->add(node);
    std::shared_ptr<wireframe_program> prog = std::make_shared<wireframe_program>();
    prog->set_solid_color(color4f(0.5f, 0.5f, 0.5f));
    prog->set_wire_color(color4f(1.0f, 1.0f, 1.0f));

    engine.set_render_callback([&](rendering_context& ctx) {
        camera->render(root, ctx, prog);
    });

    engine.set_resize_callback([&](rendering_context& ctx, sf::Event& event) {
        camera = create_camera(*window);
        glViewport(0, 0, event.size.width, event.size.height);
        sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
        window->setView(view);
    });

    engine.run(*window);
}
