#include <GL/glew.h>

#include "yae.hpp"
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
            gl_FragColor = vec4(1.0f, 10.0f*float(i)/float(max_i), 1.0f, 1.0f);
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

void main()
{
    auto engine = std::make_unique<yae::sdl_engine>();
    auto window = engine->create_simple_window();
    auto cv = yae::clipping_volume{ -2.5f, 1.5f, -1.5f, 1.5f, -1.0f, 1.0f };
    auto camera = window->create_parallel_camera(cv);

    yae::buffer_object_builder<float> v({ -3.0f, -2.0f, 2.0f, -2.0f, 2.0f, 2.0f, -3.0f, 2.0f });
    auto canvas = std::make_shared<yae::geometry<float>>(v.get_size() / 2, 2);
    canvas->set_vertex_positions(v.build());
    auto node = std::make_shared<yae::geometry_node<float>>(std::move(canvas));
    auto root = std::make_shared<yae::group>();
    root->add(node);
    std::shared_ptr<mandelbrot_program> prog = create_mandelbrot_program();

    window->set_render_callback([&](yae::rendering_context& ctx) {
        camera->render(root, ctx, prog);
    });

    window->set_resize_callback([&](yae::rendering_context& ctx) {
        int w = window->width();
        int h = window->height();
        camera = window->create_parallel_camera(cv);
        glViewport(0, 0, w, h);
    });

    engine->run(window.get());
}
