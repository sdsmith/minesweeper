#include "renderer/shader.h"

#include "renderer/opengl.h"
#include <iostream>
#include <utility>

ShaderFile::ShaderFile(std::string file_path)
    : file_path(std::move(file_path)), shader_id(0) {}

ShaderFile::~ShaderFile() {
    glDeleteShader(shader_id);
}

GLuint
ShaderFile::get_id() const {
    return shader_id;
}

void
ShaderFile::compile_shader(GLenum shader_type) {
    // Get file content
    std::string code;
    std::ifstream shader_file;
    shader_file.exceptions(std::ifstream::badbit);

    try {
        // Read contents of file into buffer
        shader_file.open(file_path);
        if (shader_file.fail()) {
            std::cout << "ERROR::SHADER::FILE - Failed to open '" << file_path << "'\n";
            return;
        }

        std::stringstream stream;
        stream << shader_file.rdbuf();
        shader_file.close();

        // Convert to string
        code = stream.str();
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE::FILE_READ_FAIL\n" << e.what() << std::endl;
    }

    // Compile shader
    const GLchar* cstr_code = static_cast<const GLchar*>(code.c_str());
    shader_id = glCreateShader(shader_type);
    GL_CHECK(glShaderSource(shader_id, 1, &cstr_code, nullptr));
    GL_CHECK(glCompileShader(shader_id));

    // Check compilation status
    GLint success;
    GL_CHECK(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success));
    if (!success) {
        GLint log_length;
        GL_CHECK(glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length));
        GLchar info_log[log_length];
        GL_CHECK(glGetShaderInfoLog(shader_id, log_length, nullptr, info_log));
        std::cout << "ERROR::SHADER::FILE::COMPILATION_FAILED\n" << info_log << std::endl;
    }
}

VertexShader::VertexShader(std::string const &file_path) : ShaderFile(file_path) {
    compile_shader(GL_VERTEX_SHADER);
}

FragmentShader::FragmentShader(std::string const &file_path) : ShaderFile(file_path) {
    compile_shader(GL_FRAGMENT_SHADER);
}

Shader::Shader(std::string const &vertex_file_path, std::string const &fragment_file_path) : program_id(0) {
    VertexShader v_shader(vertex_file_path);
    FragmentShader f_shader(fragment_file_path);

    Shader(v_shader, f_shader);
}

Shader::Shader(VertexShader const &vertex_shader, FragmentShader const &fragment_shader) : program_id(0) {
    // Link shaders and create program
    program_id = glCreateProgram();
    GL_CHECK(glAttachShader(program_id, vertex_shader.get_id()));
    GL_CHECK(glAttachShader(program_id, fragment_shader.get_id()));
    GL_CHECK(glLinkProgram(program_id));

    // Check for link errors
    GLint success;
    GL_CHECK(glGetProgramiv(program_id, GL_LINK_STATUS, &success));
    if (!success) {
        GLint log_length;
        GL_CHECK(glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length));
        GLchar info_log[log_length];
        GL_CHECK(glGetProgramInfoLog(program_id, log_length, nullptr, info_log));
        std::cout <<"ERROR::SHADER::PROGRAM::LINK_FAILED\n" << info_log << std::endl;
    }
}

GLuint
Shader::get_id() const {
    return program_id;
}

Shader::~Shader() {
    GL_CHECK(glDeleteProgram(program_id));
}

void
Shader::enable() {
    GL_CHECK(glUseProgram(program_id));
}

void
Shader::disable() {
    GL_CHECK(glUseProgram(0));
}
