#include "renderer/shader.h"

#include "renderer/opengl.h"
#include <iostream>

ShaderFile::ShaderFile(std::string const &filePath)
    : filePath(filePath), shaderID(0) {}

ShaderFile::~ShaderFile() {
    glDeleteShader(shaderID);
}

GLuint
ShaderFile::getID() const {
    return shaderID;
}

void
ShaderFile::compileShader(GLenum shaderType) {
    // Get file content
    std::string code;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::badbit);

    try {
        // Read contents of file into buffer
        shaderFile.open(filePath);
        std::stringstream stream;
        stream << shaderFile.rdbuf();
        shaderFile.close();

        // Convert to string
        code = stream.str();
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE::FILE_READ_FAIL\n" << e.what() << std::endl;
    }

    // Compile shader
    const GLchar* cstr_code = static_cast<const GLchar*>(code.c_str());
    shaderID = glCreateShader(shaderType);
    GL_CHECK(glShaderSource(shaderID, 1, &cstr_code, NULL));
    GL_CHECK(glCompileShader(shaderID));

    // Check compilation status
    GLint success;
    GL_CHECK(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success));
    if (!success) {
        GLint logLength;
        GL_CHECK(glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength));
        GLchar infoLog[logLength];
        GL_CHECK(glGetShaderInfoLog(shaderID, logLength, NULL, infoLog));
        std::cout << "ERROR::SHADER::FILE::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

VertexShader::VertexShader(std::string const &filePath) : ShaderFile(filePath) {
    compileShader(GL_VERTEX_SHADER);
}

FragmentShader::FragmentShader(std::string const &filePath) : ShaderFile(filePath) {
    compileShader(GL_FRAGMENT_SHADER);
}

Shader::Shader(std::string const &vertexFilePath, std::string const &fragmentFilePath) : programID(0) {
    VertexShader vShader(vertexFilePath);
    FragmentShader fShader(fragmentFilePath);

    Shader(vShader, fShader);
}

Shader::Shader(VertexShader const &vertexShader, FragmentShader const &fragmentShader) : programID(0) {
    // Link shaders and create program
    programID = glCreateProgram();
    GL_CHECK(glAttachShader(programID, vertexShader.getID()));
    GL_CHECK(glAttachShader(programID, fragmentShader.getID()));
    GL_CHECK(glLinkProgram(programID));

    // Check for link errors
    GLint success;
    GL_CHECK(glGetProgramiv(programID, GL_LINK_STATUS, &success));
    if (!success) {
        GLint logLength;
        GL_CHECK(glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength));
        GLchar infoLog[logLength];
        GL_CHECK(glGetProgramInfoLog(programID, logLength, NULL, infoLog));
        std::cout <<"ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
}

GLuint
Shader::getID() const {
    return programID;
}

Shader::~Shader() {
    GL_CHECK(glDeleteProgram(programID));
}

void
Shader::enable() {
    GL_CHECK(glUseProgram(programID));
}

void
Shader::disable() {
    GL_CHECK(glUseProgram(0));
}
