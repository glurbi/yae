#include <GL/glew.h>

#include "yae.hpp"
#include "shader.hpp"
#include "sdl.hpp"

class mandelbrot_program : public yae::shader_program {
public:
    mandelbrot_program(const std::map<int, std::string>& attribute_indices);
    virtual void render(const yae::geometry<float>& geometry, yae::rendering_context& ctx);
};

void mandelbrot_program::render(const yae::geometry<float>& geometry, yae::rendering_context& ctx)
{
    glPolygonMode(polygon_face, polygon_mode);
    glUseProgram(id);
    GLuint matrix_uniform = glGetUniformLocation(id, "mvp");
    glUniformMatrix4fv(matrix_uniform, 1, false, ctx.mvp().m);
    glEnableVertexAttribArray(yae::vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_positions_id());
    glVertexAttribPointer(yae::vertex_attribute::POSITION, geometry.get_dimensions(), GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.get_count());
    glDisableVertexAttribArray(yae::vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

static const std::string mandelbrot_vert = R"SHADER(
#version 330 core
uniform mat4 mvp;
in vec2 pos;
out vec2 coord;
void main(void)
{
	gl_Position = mvp * vec4(pos, 0.0f, 1.0f);
    coord = pos;
}
)SHADER";

static const std::string mandelbrot_frag = R"SHADER(
#version 330 core
in vec2 coord;
out vec4 gl_FragColor;
void main(void)
{
    vec2 c = coord;
    vec2 z = vec2(0.0f, 0.0f);
    int i = 0;
    const int max_i = 200;
    gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    for (; i < max_i; i++) {
        z = vec2(z.x*z.x - z.y*z.y, 2*z.x*z.y) + c;
        if (length(z) > 2.0f) {
            float base = log(float(i))/log(float(max_i));
            gl_FragColor = vec4(base, 1.0f-base, 1.0f-base, 1.0f);
        };
    }
}
)SHADER";

mandelbrot_program::mandelbrot_program(const std::map<int, std::string>& attribute_indices)
: shader_program(mandelbrot_vert, mandelbrot_frag, attribute_indices)
{
}

static std::shared_ptr<mandelbrot_program> create_mandelbrot_program()
{
    std::map<int, std::string> attribute_indices;
    attribute_indices[yae::vertex_attribute::POSITION] = "pos";
    return std::shared_ptr<mandelbrot_program>(new mandelbrot_program(attribute_indices));
}

int main()
{
    auto engine = std::make_unique<yae::sdl_engine>();
    auto window = engine->create_simple_window();
    auto cv = yae::clipping_volume{ -2.5f, 2.5f, -2.5f, 2.5f, -1.0f, 1.0f };
    window->close_when_keydown();

    yae::buffer_object_builder<float> v({ -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f, -5.0f, 5.0f });
    auto canvas = std::make_shared<yae::geometry<float>>(v.get_size() / 2, 2, GL_QUADS);
    canvas->set_vertex_positions(v.build());
    auto node = std::make_shared<yae::geometry_node<float>>(std::move(canvas));
    auto root = std::make_shared<yae::group>();
    root->add(node);
    std::shared_ptr<mandelbrot_program> prog = create_mandelbrot_program();

    auto scene = std::make_shared<yae::rendering_scene>();
    auto cam = std::make_shared<yae::parallel_camera>(cv);
    scene->associate_camera<yae::rendering_scene::fit_all_adapter>(cam, window.get(), yae::viewport_relative{0.0f, 0.0f, 1.0f, 1.0f});
    auto bg_color = yae::color4f{ 0.0f, 0.0f, 0.0f, 0.0f };
    auto clear_viewport_cb = yae::clear_viewport_callback(bg_color, scene->get_viewport());
    auto cre = std::make_shared<yae::custom_rendering_element>("clear_viewport", clear_viewport_cb);
    auto nre = std::make_shared<yae::node_rendering_element>("mandelbrot_canvas", root, prog, cam);
    scene->add_element(cre);
    scene->add_element(nre);

    cam->move_up(0.654f);
    cam->move_left(0.088f);
    window->set_render_callback([&](yae::rendering_context& ctx) {
        cam->open((float)(1.0 + sin(ctx.elapsed_time_seconds)*0.04));
    });

    window->add_scene(scene);

    engine->run(window.get());

    return 0;
}
