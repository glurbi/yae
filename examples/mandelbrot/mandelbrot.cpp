#include <GL/glew.h>
#include "yae.hpp"
#include "sdl_backend.hpp"

using namespace yae;

class mandelbrot_program : public shader_program {
public:
    mandelbrot_program(const std::map<int, std::string>& attribute_indices);
    virtual void render(const geometry<float>& geometry, rendering_context& ctx);
};

void mandelbrot_program::render(const geometry<float>& geometry, rendering_context& ctx)
{
    glPolygonMode(polygon_face, polygon_mode);
    glUseProgram(id);
    GLuint matrix_uniform = glGetUniformLocation(id, "mvp");
    glUniformMatrix4fv(matrix_uniform, 1, false, ctx.mvp().m);
    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_positions_id());
    glVertexAttribPointer(vertex_attribute::POSITION, geometry.get_dimensions(), GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.get_count());
    glDisableVertexAttribArray(vertex_attribute::POSITION);
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
    attribute_indices[vertex_attribute::POSITION] = "pos";
    return std::shared_ptr<mandelbrot_program>(new mandelbrot_program(attribute_indices));
}

std::unique_ptr<camera> create_camera(Wwindow* win)
{
    clipping_volume cv;
    int w = win->width();
    int h = win->height();
    float ar = (float)w / h;
    cv.right = 1.5f;
    cv.left = -2.5f;
    cv.bottom = -1.5f / ar;
    cv.top = 1.5f / ar;
    cv.nearp = -1.0f;
    cv.farp = 1.0f;
    std::unique_ptr<camera> camera = std::make_unique<parallel_camera>(cv);
    return camera;
}

void main()
{
    auto yae = ::yae::yae<sdl_backend>{};
    auto engine = ::yae::engine{};
    auto window = yae.create_simple_window();
    auto camera = create_camera(window.get());

    buffer_object_builder<float> v({ -3.0f, -2.0f, 2.0f, -2.0f, 2.0f, 2.0f, -3.0f, 2.0f });
    auto canvas = std::make_shared<geometry<float>>(v.get_size() / 2, 2);
    canvas->set_vertex_positions(v.build());
    auto node = std::make_shared<geometry_node<float>>(std::move(canvas));
    auto root = std::make_shared<group>();
    root->add(node);
    std::shared_ptr<mandelbrot_program> prog = create_mandelbrot_program();

    engine.set_render_callback([&](rendering_context& ctx) {
        camera->render(root, ctx, prog);
    });

    engine.set_resize_callback([&](rendering_context& ctx, SDL_Event event) {
        int w = window->width();
        int h = window->height();
        camera = create_camera(window.get());
        glViewport(0, 0, w, h);
    });

    engine.run(window.get());
}
