#include <GL/glew.h>
#include "yae.hpp"

std::unique_ptr<camera> create_camera(sf::RenderWindow& window)
{
    clipping_volume cv;
    float ar = (float)window.getSize().x / window.getSize().y;
    cv.right = 2.0f;
    cv.left = -2.0f;
    cv.bottom = -2.0f / ar;
    cv.top = 2.0f / ar;
    cv.nearp = 1.0f;
    cv.farp = 100.0f;
    std::unique_ptr<camera> camera = std::make_unique<perspective_camera>(cv);
    camera->move_backward(20.0f);
    return camera;
}

int main()
{
    auto yae = yae::yae{};
    auto engine = yae::engine{};
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
    auto monochrome_program = monochrome_program::create_3d();
    monochrome_program->set_polygon_mode(GL_LINE);
    monochrome_program->set_polygon_face(GL_FRONT_AND_BACK);

    engine.set_render_callback([&](rendering_context& ctx) {
        camera->render(root, ctx, monochrome_program);
    });

    engine.set_resize_callback([&](rendering_context& ctx, sf::Event& event) {
        camera = create_camera(*window);
        glViewport(0, 0, event.size.width, event.size.height);
        sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
        window->setView(view);
    });

    engine.run(*window);
}
