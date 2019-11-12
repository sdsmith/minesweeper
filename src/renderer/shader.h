#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>

// TODO(stewarts): Why do most of these functions return void???

class ShaderFile {
public:
    ShaderFile(std::string file_path);
    ~ShaderFile();

    [[nodiscard]] GLuint get_id() const;

protected:
    std::string file_path;
    GLuint shader_id;

    void compile_shader(GLenum shader_type);
};

class VertexShader : public ShaderFile {
public:
    VertexShader(std::string const &file_path);
};

class FragmentShader : public ShaderFile {
public:
    FragmentShader(std::string const &filepath);
};

class Shader {
public:
    Shader(std::string const &vertex_file_path,
           std::string const &fragment_file_path);
    Shader(VertexShader const &vertex_shader,
           FragmentShader const &fragment_shader);
    ~Shader();

    [[nodiscard]] GLuint get_id() const;
    void enable();
    static void disable();

private:
    GLuint program_id = 0;
};

#endif
