#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>

#include "yae.hpp"

using namespace yae;

struct timer::timer_private {
    static const double r;
    std::chrono::high_resolution_clock::time_point start;
    timer_private() {}
    ~timer_private() {}
};

const double timer::timer_private::r = (double) std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den;

timer::timer()
{
    p = std::unique_ptr<timer::timer_private>(new timer::timer_private());
    reset();
}

timer::~timer()
{
}
    
void timer::reset()
{
    p->start = std::chrono::high_resolution_clock::now();
}

double timer::elapsed()
{
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration duration = now - p->start;
    return duration.count() * p->r;
}

texture::texture(GLubyte* data, GLsizei w, GLsizei h)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

texture::~texture()
{
    glDeleteTextures(1, &id);
}

GLuint texture::get_id() const
{
    return id;
}

void ::yae::check_for_opengl_errors()
{
    switch (glGetError()) {
    case GL_INVALID_ENUM: std::cout << "GLenum argument out of range" << std::endl; break;
    case GL_INVALID_VALUE: std::cout << "Numeric argument out of range" << std::endl; break;
    case GL_INVALID_OPERATION: std::cout << "Operation illegal in current state" << std::endl; break;
    case GL_STACK_OVERFLOW: std::cout << "Command would cause a stack overflow" << std::endl; break;
    case GL_STACK_UNDERFLOW: std::cout << "Command would cause a stack underflow" << std::endl; break;
    case GL_OUT_OF_MEMORY: std::cout << "Not enough memory left to execute command" << std::endl; break;
    }
}

rendering_context::rendering_context()
{
    memset(last_frame_times_seconds, 0, 100);
    elapsed_time_seconds = 0.0;
    frame_count = 0;
    reset();
}

void rendering_context::projection(matrix44f mat)
{
    mvp_stack.push_back(multm(mvp_stack.back(), mat));
}

void rendering_context::push(matrix44f mat)
{
    mvp_stack.push_back(multm(mvp_stack.back(), mat));
    mv_stack.push_back(multm(mv_stack.back(), mat));
}

void rendering_context::pop()
{
    mvp_stack.pop_back();
    mv_stack.pop_back();
}

void rendering_context::reset()
{
    mvp_stack.clear();
    mv_stack.clear();
    mvp_stack.push_back(identity<float>());
    mv_stack.push_back(identity<float>());
}

matrix44f rendering_context::mvp()
{
    return mvp_stack.back();
}

matrix44f rendering_context::mv()
{
    return mv_stack.back();
}

camera::camera(const clipping_volume& cv) : cv(cv), position_v(vector3f(0, 0, 0)),
direction_v(vector3f(0, 0, -1)), right_v(vector3f(1, 0, 0)), up_v(vector3f(0, 1, 0))
{
}

void camera::reset()
{
    position_v = vector3f(0, 0, 0);
    direction_v = vector3f(0, 0, -1);
    right_v = vector3f(1, 0, 0);
    up_v = vector3f(0, 1, 0);
}

void camera::rotate_x(float deg)
{
    direction_v = normalize(direction_v * (float)cos(to_radians(deg)) + up_v * (float)sin(to_radians(deg)));
    up_v = cross_product(direction_v, right_v) * -1.0f;
}

void camera::rotate_y(float deg)
{
    direction_v = normalize(direction_v * (float)cos(to_radians(deg)) - right_v * (float)sin(to_radians(deg)));
    right_v = cross_product(direction_v, up_v);
}

void camera::rotate_z(float deg)
{
    right_v = normalize(right_v * (float)cos(to_radians(deg)) + up_v * (float)sin(to_radians(deg)));
    up_v = cross_product(direction_v, right_v) * -1.0f;
}

void camera::move_right(float dist)
{
    position_v = position_v + (right_v * dist);
}

void camera::move_left(float dist)
{
    position_v = position_v - (right_v * dist);
}

void camera::move_up(float dist)
{
    position_v = position_v + (up_v * dist);
}

void camera::move_down(float dist)
{
    position_v = position_v - (up_v * dist);
}

void camera::move_forward(float dist)
{
    position_v = position_v + (direction_v * dist);
}

void camera::move_backward(float dist)
{
    position_v = position_v - (direction_v * dist);
}

void camera::open(float factor)
{
    float delta_height = (cv.top - cv.bottom) * (1 - factor);
    cv.bottom -= delta_height;
    cv.top += delta_height;
    float delta_width = (cv.right - cv.left) * (1 - factor);
    cv.left -= delta_width;
    cv.right += delta_width;
}

void camera::set_opening(float width, float height)
{
    cv.bottom = -height / 2;
    cv.top = height / 2;
    cv.left = -width / 2;
    cv.right = width / 2;
}

float camera::get_height()
{
    return cv.top - cv.bottom;
}

float camera::get_width()
{
    return cv.right - cv.left;
}

matrix44f camera::position_and_orient()
{
    vector3f centerV = position_v + direction_v;
    return look_at(position_v.x(), position_v.y(), position_v.z(), centerV.x(), centerV.y(), centerV.z(), up_v.x(), up_v.y(), up_v.z());
}

perspective_camera::perspective_camera(const clipping_volume& cv)
: camera(cv)
{
}

void perspective_camera::render(std::shared_ptr<node> node, rendering_context& ctx, std::shared_ptr<program> prog)
{
    ctx.projection(frustum(cv.left, cv.right, cv.bottom, cv.top, cv.nearp, cv.farp));
    ctx.push(position_and_orient());
    ctx.prog = prog;
    node->render(ctx);
    ctx.reset();
}

parallel_camera::parallel_camera(const clipping_volume& clippingVolume)
: camera(clippingVolume)
{
}

void parallel_camera::render(std::shared_ptr<node> node, rendering_context& ctx, std::shared_ptr<program> prog)
{
    ctx.projection(ortho(cv.left, cv.right, cv.bottom, cv.top, cv.nearp, cv.farp));
    ctx.push(position_and_orient());
    ctx.prog = prog;
    node->render(ctx);
    ctx.reset();
}

group::group()
: transform_callback([](rendering_context& ctx) { return identity<float>(); })
{
}

void group::set_transform_callback(std::function<matrix44f(rendering_context&)> f)
{
    transform_callback = f;
}

void group::add(std::shared_ptr<node> node)
{
    children.push_back(node);
}

void group::render(rendering_context& ctx)
{
    ctx.push(transform_callback(ctx));
    for (auto child : children) {
        child->render(ctx);
    }
    ctx.pop();
}

static std::string	read_text_file(const std::string& filename)
{
    std::ifstream f(filename);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

static void check_shader_compile_status(GLuint shader_id)
{
    GLint compile_status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        printf("Shader compilation failed...\n");
        char* log = (char*)malloc((1 + info_log_length)*sizeof(char));
        glGetShaderInfoLog(shader_id, info_log_length, NULL, log);
        log[info_log_length] = 0;
        printf("%s", log);
    }
}

static void check_program_link_status(GLuint program_id)
{
    GLint link_status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLint info_log_length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        printf("Program link failed...\n");
        char* log = (char*)malloc((1 + info_log_length)*sizeof(char));
        glGetProgramInfoLog(program_id, info_log_length, NULL, log);
        log[info_log_length] = 0;
        printf("%s", log);
    }
}

template <GLenum type>
shader<type>::shader(const std::string& source)
{
    id = glCreateShader(type);
    const GLchar* str = source.c_str();
    const GLint length = source.length();
    glShaderSource(id, 1, &str, &length);
    glCompileShader(id);
    check_shader_compile_status(id);
}

template <GLenum type>
shader<type>::~shader()
{
    glDeleteShader(id);
}

template <GLenum type>
GLuint shader<type>::get_id() const
{
    return id;
}

shader_program::shader_program(const std::string& vertex_shader_source,
    const std::string& fragment_shader_source,
    const std::map<int, std::string>& attribute_indices)
    : vertex_shader(vertex_shader_source), fragment_shader(fragment_shader_source),
    polygon_face(GL_FRONT), polygon_mode(GL_FILL)
{
    id = glCreateProgram();
    glAttachShader(id, vertex_shader.get_id());
    glAttachShader(id, fragment_shader.get_id());
    for (auto it = attribute_indices.begin(); it != attribute_indices.end(); it++) {
        glBindAttribLocation(id, it->first, it->second.c_str());
    }
    glLinkProgram(id);
    check_program_link_status(id);
}

shader_program::~shader_program()
{
    glDeleteProgram(id);
}

void monochrome_program::render(const geometry<float>& geometry, rendering_context& ctx)
{
    glPolygonMode(polygon_face, polygon_mode);
    glUseProgram(id);
    GLuint matrix_uniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrix_uniform, 1, false, ctx.mvp().m);
    GLuint color_uniform = glGetUniformLocation(id, "color");
    glUniform4f(color_uniform, col.r(), col.g(), col.b(), col.a());
    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_positions_id());
    glVertexAttribPointer(vertex_attribute::POSITION, geometry.get_dimensions(), GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.get_count());
    glDisableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(id);
}

static const std::string monochrome_2d_vert = R"SHADER(
#version 330
uniform mat4 mvpMatrix;
uniform vec4 color;
in vec2 vpos;
out vec4 vcolor;
void main(void)
{
	gl_Position = mvpMatrix * vec4(vpos, 0.0f, 1.0f);
	vcolor = color;
}
)SHADER";

static const std::string monochrome_frag = R"SHADER(
#version 330
in vec4 vcolor;
out vec4 fcolor;
void main(void)
{
	fcolor = vcolor;
}
)SHADER";

static const std::string monochrome_3d_vert = R"SHADER(
#version 330
uniform mat4 mvpMatrix;
uniform vec4 color;
in vec3 vpos;
out vec4 vcolor;
void main(void)
{
	gl_Position = mvpMatrix * vec4(vpos, 1.0f);
	vcolor = color;
}
)SHADER";

monochrome_program::monochrome_program(const std::string& monochrome_vert, const std::string& monochrome_frag, const std::map<int, std::string>& attribute_indices)
: shader_program(monochrome_vert, monochrome_frag, attribute_indices)
{
}

std::shared_ptr<monochrome_program> monochrome_program::create_2d()
{
    std::map<int, std::string> monochrome_attribute_indices;
    monochrome_attribute_indices[vertex_attribute::POSITION] = "vpos";
    return std::shared_ptr<monochrome_program>(new monochrome_program(monochrome_2d_vert, monochrome_frag, monochrome_attribute_indices));
}

std::shared_ptr<monochrome_program> monochrome_program::create_3d()
{
    std::map<int, std::string> monochrome_attribute_indices;
    monochrome_attribute_indices[vertex_attribute::POSITION] = "vpos";
    return std::shared_ptr<monochrome_program>(new monochrome_program(monochrome_3d_vert, monochrome_frag, monochrome_attribute_indices));
}

void texture_program::render(const geometry<float>& geometry, rendering_context& ctx)
{
    glUseProgram(id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, current_texture->get_id());
    GLuint matrix_uniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrix_uniform, 1, false, ctx.mvp().m);
    GLuint texture_uniform = glGetUniformLocation(id, "texture");
    glUniform1i(texture_uniform, 0); // we pass the texture unit
    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_positions_id());
    glVertexAttribPointer(vertex_attribute::POSITION, geometry.get_dimensions(), GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertex_attribute::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_tex_coords_id());
    glVertexAttribPointer(vertex_attribute::TEXCOORD, geometry.get_dimensions(), GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(vertex_attribute::POSITION);
    glDisableVertexAttribArray(vertex_attribute::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void texture_program::set_texture(std::shared_ptr<texture> t)
{
    current_texture = t;
}

std::shared_ptr<texture_program> texture_program::create()
{
    std::map<int, std::string> texture_attribute_indices;
    texture_attribute_indices[vertex_attribute::POSITION] = "pos";
    texture_attribute_indices[vertex_attribute::TEXCOORD] = "texCoord";
    return std::shared_ptr<texture_program>(new texture_program(texture_attribute_indices));
}

static const std::string texture_vert = R"SHADER(
#version 330 core
uniform mat4 mvpMatrix;
uniform sampler2D texture;
in vec2 pos;
in vec2 texCoord;
out vec2 vTexCoord;
void main(void)
{
	gl_Position = mvpMatrix * vec4(pos, 0.0f, 1.0f);
    vTexCoord = texCoord;
}
)SHADER";

static const std::string texture_frag = R"SHADER(
#version 330 core
uniform sampler2D texture;
in vec2 vTexCoord;
out vec4 fColor;
void main(void)
{
    fColor = texture2D(texture, vTexCoord);
}
)SHADER";

texture_program::texture_program(std::map<int, std::string>& attribute_indices) :
shader_program(texture_vert, texture_frag, attribute_indices) {}

void flat_shading_program::render(const geometry<float>& geometry, rendering_context& ctx)
{
    glUseProgram(id);

    GLuint mvp_uniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(mvp_uniform, 1, false, ctx.mvp().m);

    GLuint mv_uniform = glGetUniformLocation(id, "mvMatrix");
    glUniformMatrix4fv(mv_uniform, 1, false, ctx.mv().m);

    GLuint light_dir_uniform = glGetUniformLocation(id, "lightDir");
    glUniform3f(light_dir_uniform, ctx.dir.v[0], ctx.dir.v[1], ctx.dir.v[2]);

    GLuint color_uniform = glGetUniformLocation(id, "color");
    glUniform3f(color_uniform, col.r(), col.g(), col.b());

    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_positions_id());
    glVertexAttribPointer(vertex_attribute::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertex_attribute::NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_normals_id());
    glVertexAttribPointer(vertex_attribute::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.get_count());
    glDisableVertexAttribArray(vertex_attribute::POSITION);
    glDisableVertexAttribArray(vertex_attribute::NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::shared_ptr<flat_shading_program> flat_shading_program::create()
{
    std::map<int, std::string> attribute_indices;
    attribute_indices[vertex_attribute::POSITION] = "vPosition";
    attribute_indices[vertex_attribute::NORMAL] = "vNormal";
    return std::shared_ptr<flat_shading_program>(new flat_shading_program(attribute_indices));
}

static const std::string flat_shading_vert = R"SHADER(
#version 330 core
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform vec3 color;
uniform vec3 lightDir;
in vec3 vPosition;
in vec3 vNormal;
out vec4 vColor;
void main(void)
{
    vec3 normalEye;
    float dotProduct;
    /* We transform the normal in eye coordinates. */
    normalEye = vec3(mvMatrix * vec4(vNormal, 0.0f));
    /* We compute the dot product of the normal in eye coordinates by the light direction.
    The value will be positive when the diffuse light should be ignored, negative otherwise. */
    dotProduct = dot(normalEye, lightDir);
    gl_Position = mvpMatrix * vec4(vPosition, 1.0f);
    vColor = -min(dotProduct, 0.0f) * vec4(color, 1.0f);
}
)SHADER";

const std::string flat_shading_frag = R"SHADER(
#version 330 core
in vec4 vColor;
out vec4 fColor;
void main(void)
{
    fColor = vColor;
}
)SHADER";

flat_shading_program::flat_shading_program(const std::map<int, std::string>& attribute_indices) :
shader_program(flat_shading_vert, flat_shading_frag, attribute_indices) {}

wireframe_program::wireframe_program()
: prog(monochrome_program::create_3d())
{
}

void wireframe_program::render(const geometry<float>& geometry, rendering_context& ctx)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    prog->set_polygon_mode(GL_FILL);
    prog->set_polygon_face(GL_FRONT_AND_BACK);
    prog->set_color(solid_col);
    prog->render(geometry, ctx);
    glDisable(GL_POLYGON_OFFSET_FILL);
    prog->set_polygon_mode(GL_LINE);
    prog->set_color(wire_col);
    prog->render(geometry, ctx);
}

struct engine::impl {
    timer timer_absolute;
    timer timer_frame;
    rendering_context ctx;
    std::function<void(rendering_context&)> render_callback;
    std::function<void(rendering_context&)> resize_callback;
    void run(window* win, ::yae::yae& yae);
};

engine::engine()
: pimpl(std::make_unique<impl>())
{
}

engine::engine(engine&& e)
{
    pimpl = std::move(e.pimpl);
}

engine::~engine()
{
}

void engine::run(window* win, ::yae::yae& yae)
{
    pimpl->run(win, yae);
}

void engine::set_render_callback(std::function<void(rendering_context&)> f)
{
    pimpl->render_callback = f;
}

void engine::set_resize_callback(std::function<void(rendering_context&)> f)
{
    pimpl->resize_callback = f;
}

void engine::impl::run(window* win, ::yae::yae& yae)
{
    while (true) {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count % 100] = timer_frame.elapsed();
        timer_frame.reset();
        check_for_opengl_errors();
        std::vector<::yae::event> events = win->events();
        for (::yae::event e : events) {
            if (e.value == yae.quit() || e.value == yae.keydown()) {
                return;
            }
            else if (e.value == yae.window_resized()) {
                resize_callback(ctx);
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_callback(ctx);
        win->swap();
        ctx.frame_count++;
    }
}

std::unique_ptr<camera> window::create_perspective_camera(
    float right, float left, float top, float bottom, float nearp, float farp)
{
    clipping_volume cv;
    int w = width();
    int h = height();
    float ar = (float)w / h;
    cv.right = right;
    cv.left = left;
    cv.bottom = bottom / ar;
    cv.top = top / ar;
    cv.nearp = nearp;
    cv.farp = farp;
    std::unique_ptr<camera> camera = std::make_unique<perspective_camera>(cv);
    camera->move_backward(20.0f);
    return camera;
}

std::unique_ptr<camera> window::create_parallel_camera(
    float right, float left, float top, float bottom, float nearp, float farp)
{
    clipping_volume cv;
    int w = width();
    int h = height();
    float ar = (float)w / h;
    cv.right = right;
    cv.left = left;
    cv.bottom = bottom / ar;
    cv.top = top / ar;
    cv.nearp = nearp;
    cv.farp = farp;
    std::unique_ptr<camera> camera = std::make_unique<parallel_camera>(cv);
    return camera;
}

