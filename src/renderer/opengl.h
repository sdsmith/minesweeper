#pragma once

#include "renderer/renderer.h"
#include "renderer/shader.h"
#include <glm/glm.hpp>
#define GLEW_STATIC // Use GLEW static library
#include <GL/glew.h>
#include <string>
#include <cstddef>

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
    GLuint vao = -1;
    Shader* shader = nullptr;
    GLuint textures[2];

    GLfloat calcFrustumScale(GLfloat fovDegree);

public:
    OpenGl(Platform *platform);
    OpenGl(const OpenGl& o) = delete;

    static bool checkGlError(std::string const &description, std::string const &file, std::size_t const line);
    void clearScreen();
    void swapBuffer();
    void setVsync(bool enable);
    void setWindowSize(u32 w, u32 h);
    void protoSetup();
    void protoDraw();

    OpenGl& operator=(const OpenGl& o) = delete;
};
