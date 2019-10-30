#include "platform/sdl2.h"

#include <system_error>

Sdl2::Sdl2()
    : window(nullptr), glContext(nullptr), input(), newInput(input), oldInput(input + 1) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw new std::system_error(std::error_code(),
                                    "SDL initialization failure: " + std::string(SDL_GetError()) + '\n');
    }

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "SDL initialized\n");
}

Sdl2::~Sdl2() {
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

constexpr Game_Input Sdl2::zeroedInput = {};

void
Sdl2::setWindowSize(u32 w, u32 h) {
    SDL_SetWindowSize(window, static_cast<int>(w), static_cast<int>(h));
}

void
Sdl2::createOpenGlRenderingContext(s32 glMajorVersion, s32 glMinorVersion,
                                   s32 windowWidth, s32 windowHeight) {
    window = SDL_CreateWindow("RUINATION",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM,
                        "SDL failed to create window: %s\n", SDL_GetError());
        throw new std::system_error(std::error_code(), "window creation failure\n");
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "window created\n");

    // Use the new OpenGL Standard
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Set OpenGL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinorVersion);

    // Double buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Link rendering context
    glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                        "failed to set OpenGL rending context to window: %s\n", SDL_GetError());
    }

    // Turn off VSync - it will be controlled from the renderer
    if (SDL_GL_SetSwapInterval(0) < 0)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                        "failed to set swap interval: %s\n", SDL_GetError());
    }

}

void
Sdl2::swapWindowBuffer() {
    SDL_GL_SwapWindow(window);
}

void
Sdl2::processSysEventQueue() {
    Game_State* state = &newInput->state;
    SDL_Event event;

    // Setup for new system input
    prepareForNewInput();

    // Loop through waiting events
    while(SDL_PollEvent(&event))
    {
	// Handle event
	switch (event.type) {
	case SDL_QUIT: {
            state->request_quit = true;
        } break;

	case SDL_KEYDOWN: {
            processKeyboardEvent(&event, true);
        } break;

	case SDL_KEYUP: {
            processKeyboardEvent(&event, false);
        } break;

	case SDL_WINDOWEVENT: {
            processWindowEvent(&event.window);
        } break;

	default: {
        } break;
	}
    }
}

Game_Input*
Sdl2::getInput() {
    return newInput;
}

void
Sdl2::prepareForNewInput() {
    // Save old input state
    std::swap(newInput, oldInput);

    Game_Input_Controller* newKeyboard = &newInput->controllers[Controller::keyboard];
    Game_Input_Controller* oldKeyboard = &oldInput->controllers[Controller::keyboard];

    // Zero the new keyboard
    *newKeyboard = zeroedInput.controllers[0];

    Game_Input_Button* newButton;
    Game_Input_Button* oldButton;

    for (std::size_t buttonIndex = 0;
         buttonIndex < sizeof(newKeyboard->buttons)/sizeof(newKeyboard->buttons[0]); buttonIndex++) {
        newButton = &newKeyboard->buttons[buttonIndex];
        oldButton = &oldKeyboard->buttons[buttonIndex];

        // We start down if we ended down on last input
        newButton->started_down = oldButton->ended_down;

        // Since input has not been processed yet, there are no half transitions
        // ie. We ended down if we started down (updated during input processing)
        newButton->ended_down = newButton->started_down;
    }
}

u32
Sdl2::getTicks() {
    return SDL_GetTicks();
}

u64
Sdl2::getPerformanceFrequency() {
    return SDL_GetPerformanceFrequency();
}

u64
Sdl2::getPerformanceCounter() {
    return SDL_GetPerformanceCounter();
}

void
Sdl2::processWindowEvent(SDL_WindowEvent* event) {
    // TODO(sdsmith): port
    Game_State* state = &newInput->state;

    if (SDL_GetWindowID(window) == event->windowID)
    {
        // NOTE(sdsmith): https://wiki.libsdl.org/SDL_WindowEventID
        switch (event->event) {
	case SDL_WINDOWEVENT_SHOWN: {
        } break;

	case SDL_WINDOWEVENT_HIDDEN: {
            // TODO(sdsmith): stop drawing
        } break;

	case SDL_WINDOWEVENT_EXPOSED: {
            // TODO(sdsmith): redraw
        } break;

	case SDL_WINDOWEVENT_SIZE_CHANGED: {
            // NOTE(sdsmith): window size has changed
            // TODO(sdsmith): assuming it has changed to data1 by data2
            //reshape(event->data1, window_event->data2);
        } break;

	case SDL_WINDOWEVENT_MINIMIZED: {
            // TODO(sdsmith): pause game
        } break;

	case SDL_WINDOWEVENT_MAXIMIZED: {
            // TODO(sdsmith): run game. (will this be followed by a shown?)
        } break;

	case SDL_WINDOWEVENT_ENTER: {
            // TODO(sdsmith): mouse has entered
        } break;

	case SDL_WINDOWEVENT_LEAVE: {
            // TODO(sdsmith): mouse has left
        } break;

	case SDL_WINDOWEVENT_FOCUS_GAINED: {
            // TODO(sdsmith): keyboard entered
        } break;

	case SDL_WINDOWEVENT_FOCUS_LOST: {
            // TODO(sdsmith): keyboard lost
        } break;

	case SDL_WINDOWEVENT_CLOSE: {
            // TODO(sdsmith): is this a duplicate of SDL_QUIT event?
            state->request_quit = true;
        } break;

	default: {
        } break;
	}
    }
}

void
Sdl2::processKeyboardEvent(SDL_Event* event, bool key_down) {
    Game_Input_Controller* keyboard = &newInput->controllers[Controller::keyboard];
    SDL_Keycode key = event->key.keysym.sym;

    switch(key) {
    case SDLK_w: {
        processInputButton(&keyboard->up, key_down);
    } break;

    case SDLK_a: {
        processInputButton(&keyboard->left, key_down);
    } break;

    case SDLK_s: {
        processInputButton(&keyboard->down, key_down);
    } break;

    case SDLK_d: {
        processInputButton(&keyboard->right, key_down);
    } break;

    default: {
    } break;
    }
}

void
Sdl2::processInputButton(Game_Input_Button* button, bool button_down)
{
    button->ended_down = button_down;
    button->half_transitions++;
}
