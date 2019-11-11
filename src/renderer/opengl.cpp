#include "renderer/opengl.h"

#include "logger.h"
#include <glm/gtc/type_ptr.hpp> // value_ptr
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#ifdef _WIN32
#    include <GL/wglew.h> // Windows OpenGL extensions
#elif __linux__
#    include <GL/glxew.h> // Linux OpenGL extensions
#else
#    error "Unsupported OS"
#endif

#define __USE_MATH_DEFINES // For compatibility with old cmath #defines
#include <cmath>
#include <iostream>

OpenGl::OpenGl(char const* window_name, Platform* platform) : platform(platform)
{
    platform->create_open_gl_rendering_context(window_name, 3, 3, 640, 480);

    // Enable expiremental functionality
    glewExperimental = GL_TRUE;
    // Load available OpenGl functions
    glewInit();

    // NOTE(sdsmith): GLEW may return GL_INVALID_ENUM error (issue with openGL
    // core profile and glew). Reset the error flag in case this occurs.
    GLenum gl_error = glGetError();
    if (gl_error != GL_NO_ERROR && gl_error != GL_INVALID_ENUM) {
        // An unexpected GL error occured from GLEW
        std::cout << "ERROR::OPENGL::INIT::GLEW::UNEXPECTED_GL_ERROR"
                  << std::endl;
        throw new std::system_error(
            std::error_code(),
            "Unexpected OpenGL init failure\n"); // TODO(stewarts): better error
                                                 // handling
    }

    // Disbale Vsync by default
    set_vsync(true);
}

bool OpenGl::check_gl_error(std::string const& description,
                            std::string const& file, std::size_t const line)
{
    GLenum error = glGetError();
    std::string log;

    // Check if there was initially no error
    if (error == GL_NO_ERROR) { return false; }

    // Display each error in queue
    while (error != GL_NO_ERROR) {
        switch (error) {
            case GL_INVALID_ENUM: {
                log = "GL_INVALID_ENUM";
            } break;

            case GL_INVALID_VALUE: {
                log = "GL_INVALID_VALUE";
            } break;

            case GL_INVALID_OPERATION: {
                log = "GL_INVALID_OPERATION";
            } break;

            case GL_INVALID_FRAMEBUFFER_OPERATION: {
                log = "GL_INVALID_FRAMEBUFFER_OPERATION";
            } break;

            case GL_OUT_OF_MEMORY: {
                log = "GL_OUT_OF_MEMORY";
            } break;

            case GL_STACK_UNDERFLOW: {
                log = "GL_STACK_UNDERFLOW";
            } break;

            case GL_STACK_OVERFLOW: {
                log = "GL_STACK_OVERFLOW";
            } break;

            default: {
                log = "<UNKNOWN>";
            } break;
        }

        logCritical(LOG_VIDEO, "OpenGL Error: %s - %s\n\t%s, line %d\n",
                    log.c_str(), description.c_str(), file.c_str(),
                    static_cast<int>(line));

        error = glGetError();
    }

    return true;
}

void OpenGl::set_window_size(u32 w, u32 h) { platform->set_window_size(w, h); }

void OpenGl::clear_screen()
{
    GL_CHECK(glClearColor(1.0, 0.0, 0.5, 1.0));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
}

void OpenGl::proto_setup()
{
    // Build and compile our shader program
    VertexShader v_shader("res/shaders/basic.vert");
    FragmentShader f_shader("res/shaders/basic.frag");

    shader = std::make_unique<Shader>(v_shader, f_shader);

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat constexpr vertices[] = {
        // Position          //Color            // Texture Coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Top Left
    };
    GLuint constexpr indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    GLuint vbo;
    GLuint ebo;
    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glGenBuffers(1, &ebo));
    GL_CHECK(glGenVertexArrays(1, &vao));
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s)
    // and attribute pointer(s).
    GL_CHECK(glBindVertexArray(vao));
    {
        // Copy vertices to vertex buffer
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                              GL_STATIC_DRAW));

        // Copy indices to element buffer
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
        GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                              GL_STATIC_DRAW));

        // Set position attribute
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                       8 * sizeof(GLfloat),
                                       static_cast<GLvoid*>(nullptr)));
        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
            reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat))));
        GL_CHECK(glEnableVertexAttribArray(1));
        GL_CHECK(glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
            reinterpret_cast<GLvoid*>(6 * sizeof(GLfloat))));
        GL_CHECK(glEnableVertexAttribArray(2));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER,
                              0)); // NOTE(sdsmith): vbo bound to vao after
                                   // glVertexAttribPointer call
    }
    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    ////////// Texture
    GL_CHECK(glGenTextures(2, textures));

    //// Create texture 1
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textures[0]));
    // Set wrap behavior
    GL_CHECK(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    // Set sampling behavior
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                             GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // Load image
    int tex_width = 0, tex_height = 0, bytes_per_pixel = 0;
    unsigned char* image = stbi_load("res/textures/container.jpg", &tex_width,
                                     &tex_height, &bytes_per_pixel, 0);
    if (image == nullptr) {
        std::cout << "ERROR::TEXTURE::IMAGE::LOAD_FAILED" << std::endl;
    }
    // Generate texture
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0,
                          GL_RGB, GL_UNSIGNED_BYTE, image));
    // Generate mipmap
    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    // Cleanup
    stbi_image_free(image);
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    //// Create texture 2
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textures[1]));
    // Set wrap behavior
    GL_CHECK(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    // Set sampling behavior
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                             GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // Load image
    stbi_set_flip_vertically_on_load(true); // flip the image
    image = stbi_load("res/textures/awesomeface.jpg", &tex_width, &tex_height,
                      &bytes_per_pixel, 0);
    if (image == nullptr) {
        std::cout << "ERROR::TEXTURE::IMAGE::LOAD_FAILED" << std::endl;
    }
    // Generate texture
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0,
                          GL_RGB, GL_UNSIGNED_BYTE, image));
    // Generate mipmap
    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    // Cleanup
    stbi_image_free(image);
    stbi_set_flip_vertically_on_load(false);
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

// // TODO(sdsmith): How do we want to handle the textureIDs? Do we allocate all
// // the textures at once, or one at a time? Do we allocate them inside this
// // function, or prior to calling it?
// struct TextureDescription {
//     GLuint ID;
//     std::string filename;
//     GLint internalFormat;
//     GLint resultFormat;
//     GLint wrapS;
//     GLint wrapT;
//     GLint minFilter;
//     GLint maxFilter;
// }

// void
// OpenGl::loadTexture2D(Gluint textureID, std::string filename,
//                       GLint internalFormat, GLint resultFormat,
//                       GLint wrapS, GLint wrapT, GLint minFilter, GLint
//                       maxFilter) {

// }

void OpenGl::proto_draw()
{
    // Render
    // Clear the colorbuffer
    GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

    shader->enable();

    // Bind texture with texture units
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textures[0]));
    GL_CHECK(glUniform1i(glGetUniformLocation(shader->get_id(), "tex1"), 0));
    GL_CHECK(glActiveTexture(GL_TEXTURE1));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textures[1]));
    GL_CHECK(glUniform1i(glGetUniformLocation(shader->get_id(), "tex2"), 1));

    GL_CHECK(glBindVertexArray(vao));
    GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    GL_CHECK(glBindVertexArray(0));
}

GLfloat OpenGl::calc_frustum_scale(GLfloat fov_degree)
{
    const GLfloat degree_to_radian = static_cast<GLfloat>(M_PI * 2.0f / 360.0f);
    GLfloat fov_radian = fov_degree * degree_to_radian;

    return 1.0f / tan(fov_radian / 2.0f);
}

void OpenGl::swap_buffer() { platform->swap_window_buffer(); }

void OpenGl::set_vsync(bool enable)
{
    // NOTE(sdsmith): By setting swap interval to 1, it implies setting it to 1
    // video frame period (ie. the time required by the monitor to display a
    // full frame of video data).

    int const interval = (enable ? 1 : 0);

#ifdef _WIN32
    if (WGLEW_EXT_swap_control) {
        GL_CHECK(wglSwapIntervalExt(interval));
    }
#elif __linux__
    if (GLXEW_EXT_swap_control) {
        Display* dpy;
        GLXDrawable drawable;
        GL_CHECK(dpy = glXGetCurrentDisplay());
        GL_CHECK(drawable = glXGetCurrentDrawable());
        GL_CHECK(glXSwapIntervalEXT(dpy, drawable, interval));
    } else if (GLXEW_MESA_swap_control) {
        GL_CHECK(glXSwapIntervalMESA(interval));
    } else if (GLXEW_SGI_swap_control) {
        GL_CHECK(glXSwapIntervalSGI(static_cast<unsigned int>(interval)));
    }
#endif
    else {
        // TODO(sdsmith): fall back on SDL. First, figure out a way to keep SDL
        // decoupled
    }
}
