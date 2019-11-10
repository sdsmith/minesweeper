#pragma once

#include "renderer/renderer.h"
#include "renderer/shader.h"
#include <glm/glm.hpp>
#define GLEW_STATIC // Use GLEW static library
#include <GL/glew.h>
#include <string>
#include <cstddef>
#include <limits>

#ifdef _DEBUG
#define CHECK_GL_ERROR(description) OpenGL::checkGLError(description, __FILE__, __LINE__)
#define GL_CHECK(statement) do { statement; CHECK_GL_ERROR(#statement); } while (0)
#else
#define CHECK_GL_ERROR(description)
#define GL_CHECK(statement) statement
#endif

class OpenGl : public Renderer {
private:
    Platform* platform;

    // TEMP
    GLuint vao = std::numeric_limits<GLuint>::max();
    Shader* shader = nullptr;
    GLuint textures[2];

    GLfloat calc_frustum_scale(GLfloat fov_degree);

public:
    OpenGl(Platform *platform);
    OpenGl(const OpenGl& o) = delete;

    static bool check_gl_error(std::string const &description, std::string const &file, std::size_t const line);
    void clear_screen();
    void swap_buffer();
    void set_vsync(bool enable);
    void set_window_size(u32 w, u32 h);
    void proto_setup();
    void proto_draw();

    OpenGl& operator=(const OpenGl& o) = delete;
};
