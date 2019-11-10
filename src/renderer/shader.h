#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>


class ShaderFile {
public:
    ShaderFile(std::string const &file_path);
    ~ShaderFile(void);

    GLuint get_id(void) const;
    
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
    Shader(std::string const &vertex_file_path, std::string const &fragment_file_path);
    Shader(VertexShader const &vertex_shader, FragmentShader const &fragment_shader);
    ~Shader(void);

    GLuint get_id(void) const;
    void enable(void);
    static void disable(void);
    
private:
    GLuint program_id;
};

#endif
