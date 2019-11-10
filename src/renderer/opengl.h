#pragma once

#include "renderer/renderer.h"
#include "renderer/shader.h"
#include <glm/glm.hpp>
#define GLEW_STATIC // Use GLEW static library
#include <GL/glew.h>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>

#ifdef _DEBUG
#define CHECK_GL_ERROR(description) \
    OpenGL::checkGLError(description, __FILE__, __LINE__)
#define GL_CHECK(statement)         \
    do {                            \
        statement;                  \
        CHECK_GL_ERROR(#statement); \
    } while (0)
#else
#define CHECK_GL_ERROR(description)
#define GL_CHECK(statement) statement
#endif

// TODO(stewarts): Why do most of these functions return void???
class OpenGl : public Renderer {
private:
    Platform* platform;

    // TEMP
    GLuint vao = std::numeric_limits<GLuint>::max();
    std::unique_ptr<Shader> shader;
    GLuint textures[2];

    GLfloat calc_frustum_scale(GLfloat fov_degree);

public:
    OpenGl(char const* window_name, Platform* platform);
    OpenGl(const OpenGl& o) = delete;

    static bool check_gl_error(std::string const& description,
                               std::string const& file, std::size_t const line);
    void clear_screen() override;
    void swap_buffer() override;
    void set_vsync(bool enable);
    void set_window_size(u32 w, u32 h) override;
    void proto_setup() override;
    void proto_draw() override;

    OpenGl& operator=(const OpenGl& o) = delete;
};
