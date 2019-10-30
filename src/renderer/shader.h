#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>


class ShaderFile {
public:
    ShaderFile(std::string const &filePath);
    ~ShaderFile(void);

    GLuint getID(void) const;
    
protected:
    std::string filePath;
    GLuint shaderID;

    void compileShader(GLenum shaderType);
};


class VertexShader : public ShaderFile {
public:
    VertexShader(std::string const &filePath);
};


class FragmentShader : public ShaderFile {
public:
    FragmentShader(std::string const &filepath);
};


class Shader {
public:
    Shader(std::string const &vertexFilePath, std::string const &fragmentFilePath);
    Shader(VertexShader const &vertexShader, FragmentShader const &fragmentShader);
    ~Shader(void);

    GLuint getID(void) const;
    void enable(void);
    static void disable(void);
    
private:
    GLuint programID;
};

#endif
