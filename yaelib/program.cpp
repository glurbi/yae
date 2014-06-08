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
                 const std::map<int, std::string>& attributeIndices) :
    vertex_shader(vertexShaderSource),
    fragment_shader(fragmentShaderSource)
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
    glUseProgram(id);
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, ctx.mvp().m);
    GLuint colorUniform = glGetUniformLocation(id, "color");
    glUniform4f(colorUniform, col.r(), col.g(), col.b(), col.a());
    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_positions_id());
    glVertexAttribPointer(vertex_attribute::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
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

monochrome_program::monochrome_program(const std::map<int, std::string>& attributeIndices) :
    program(read_text_file("monochrome.vert"), read_text_file("monochrome.frag"), attributeIndices) {}

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
    glVertexAttribPointer(vertex_attribute::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertex_attribute::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.get_tex_coords_id());
    glVertexAttribPointer(vertex_attribute::TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
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

texture_program::texture_program(std::map<int, std::string>& attributeIndices) :
    program(read_text_file("texture.vert"), read_text_file("texture.frag"), attributeIndices) {}

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

flat_shading_program::flat_shading_program(const std::map<int, std::string>& attributeIndices) :
    program(read_text_file("flatShading.vert"), read_text_file("flatShading.frag"), attributeIndices) {}
