#include <iostream>
#include <sstream>
#include <fstream>

#include "yae.hpp"
#include "shader.hpp"

using namespace yae;

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
    glDrawArrays(geometry.get_primitive_type(), 0, geometry.get_count());
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
    glDrawArrays(geometry.get_primitive_type(), 0, 4);
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
    glUniform3f(light_dir_uniform, ctx.dir.x(), ctx.dir.y(), ctx.dir.z());

    GLuint color_uniform = glGetUniformLocation(id, "color");
    glUniform3f(color_uniform, col.r(), col.g(), col.b());

    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_positions_id());
    glVertexAttribPointer(vertex_attribute::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertex_attribute::NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_normals_id());
    glVertexAttribPointer(vertex_attribute::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(geometry.get_primitive_type(), 0, geometry.get_count());
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
