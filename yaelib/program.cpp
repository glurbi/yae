#include <fstream>
#include <sstream>

#include "program.hpp"
#include "misc.hpp"
#include "context.hpp"

static std::string	read_text_file(const std::string& filename) {
    std::ifstream f(filename);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

static void check_shader_compile_status(GLuint shaderId) {
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        printf("Shader compilation failed...\n");
        char* log = (char*) malloc((1+infoLogLength)*sizeof(char));
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, log);
        log[infoLogLength] = 0;
        printf("%s", log);
    }
}

static void checkProgramLinkStatus(GLuint programId) {
    GLint linkStatus;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
        printf("Program link failed...\n");
        char* log = (char*) malloc((1+infoLogLength)*sizeof(char));
        glGetProgramInfoLog(programId, infoLogLength, NULL, log);
        log[infoLogLength] = 0;
        printf("%s", log);
    }
}

template <GLenum type>
shader<type>::shader(const std::string& source) {
    id = glCreateShader(type);
    const GLchar* str = source.c_str();
    const GLint length = source.length();
    glShaderSource(id, 1, &str, &length);
    glCompileShader(id);
    check_shader_compile_status(id);
}

template <GLenum type>
shader<type>::~shader() {
    glDeleteShader(id);
}

template <GLenum type>
GLuint shader<type>::get_id() const {
    return id;
}

program::program(const std::string& vertexShaderSource,
                 const std::string& fragmentShaderSource,
                 const std::map<int, std::string>& attributeIndices)
    : vertex_shader(vertexShaderSource), fragment_shader(fragmentShaderSource),
      polygon_face(GL_FRONT), polygon_mode(GL_FILL)
{
    id = glCreateProgram();
    glAttachShader(id, vertex_shader.get_id());
    glAttachShader(id, fragment_shader.get_id());
    for (auto it = attributeIndices.begin(); it != attributeIndices.end(); it++) {
        glBindAttribLocation(id, it->first, it->second.c_str());
    }
    glLinkProgram(id);
    checkProgramLinkStatus(id);
}

program::~program() {
    glDeleteProgram(id);
}

void monochrome_program::render(const geometry<float>& geometry, rendering_context& ctx) {
    glPolygonMode(polygon_face, polygon_mode);
    glUseProgram(id);
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, ctx.mvp().m);
    GLuint colorUniform = glGetUniformLocation(id, "color");
    glUniform4f(colorUniform, col.r(), col.g(), col.b(), col.a());
    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_positions_id());
    glVertexAttribPointer(vertex_attribute::POSITION, geometry.get_dimensions(), GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.get_count());
    glDisableVertexAttribArray(vertex_attribute::POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(id);
}

std::shared_ptr<monochrome_program> monochrome_program::create() {
    std::map<int, std::string> monochromeAttributeIndices;
    monochromeAttributeIndices[vertex_attribute::POSITION] = "vpos";
    return std::shared_ptr<monochrome_program>(new monochrome_program(monochromeAttributeIndices));
}

static const std::string monochrome_vert = R"SHADER(
#version 330

uniform mat4 mvpMatrix;
uniform vec4 color;

in vec2 vpos;

out vec4 vcolor;

void main(void) {
	gl_Position = mvpMatrix * vec4(vpos, 0.0f, 1.0f);
	vcolor = color;
}
)SHADER";

static const std::string monochrome_frag = R"SHADER(
#version 330

in vec4 vcolor;

out vec4 fcolor;

void main(void) {
	fcolor = vcolor;
}
)SHADER";

monochrome_program::monochrome_program(const std::map<int, std::string>& attributeIndices) :
    program(monochrome_vert, monochrome_frag, attributeIndices) {}

void texture_program::render(const geometry<float>& geometry, rendering_context& ctx) {
    glUseProgram(id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, current_texture->get_id());
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, ctx.mvp().m);
    GLuint textureUniform = glGetUniformLocation(id, "texture");
    glUniform1i(textureUniform, 0); // we pass the texture unit
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

void texture_program::set_texture(std::shared_ptr<texture> t) {
    current_texture = t;
}

std::shared_ptr<texture_program> texture_program::create() {
    std::map<int, std::string> textureAttributeIndices;
    textureAttributeIndices[vertex_attribute::POSITION] = "pos";
    textureAttributeIndices[vertex_attribute::TEXCOORD] = "texCoord";
    return std::shared_ptr<texture_program>(new texture_program(textureAttributeIndices));
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

texture_program::texture_program(std::map<int, std::string>& attributeIndices) :
    program(texture_vert, texture_frag, attributeIndices) {}

void flat_shading_program::render(const geometry<float>& geometry, rendering_context& ctx) {
    glUseProgram(id);

    GLuint mvpUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(mvpUniform, 1, false, ctx.mvp().m);

    GLuint mvUniform = glGetUniformLocation(id, "mvMatrix");
    glUniformMatrix4fv(mvUniform, 1, false, ctx.mv().m);

    GLuint lightDirUniform = glGetUniformLocation(id, "lightDir");
    glUniform3f(lightDirUniform, ctx.dir.v[0], ctx.dir.v[1], ctx.dir.v[2]);

    GLuint colorUniform = glGetUniformLocation(id, "color");
    glUniform3f(colorUniform, col.r(), col.g(), col.b());

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

std::shared_ptr<flat_shading_program> flat_shading_program::Create() {
    std::map<int, std::string> attributeIndices;
    attributeIndices[vertex_attribute::POSITION] = "vPosition";
    attributeIndices[vertex_attribute::NORMAL] = "vNormal";
    return std::shared_ptr<flat_shading_program>(new flat_shading_program(attributeIndices));
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

flat_shading_program::flat_shading_program(const std::map<int, std::string>& attributeIndices) :
    program(flat_shading_vert, flat_shading_frag, attributeIndices) {}
