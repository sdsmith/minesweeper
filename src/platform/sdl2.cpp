#include "platform/sdl2.h"

#include <cassert>
#include <system_error>

Sdl2::Sdl2() : input(), new_input(input), old_input(input + 1)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw new std::system_error(std::error_code(),
                                    "SDL initialization failure: " +
                                        std::string(SDL_GetError()) + '\n');
    }

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "SDL initialized\n");
}

Sdl2::~Sdl2()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

constexpr Game_Input Sdl2::zeroed_input = {};

void Sdl2::set_window_size(u32 w, u32 h)
{
    SDL_SetWindowSize(window, static_cast<int>(w), static_cast<int>(h));
}

void Sdl2::create_open_gl_rendering_context(char const* window_name,
                                            s32 gl_major_version,
                                            s32 gl_minor_version,
                                            s32 window_width, s32 window_height)
{
    window = SDL_CreateWindow(
        window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM,
                        "SDL failed to create window: %s\n", SDL_GetError());
        throw new std::system_error(std::error_code(),
                                    "window creation failure\n");
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "window created\n");

    // Use the new OpenGL Standard
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    // Set OpenGL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version);

    // Double buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Link rendering context
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                        "failed to set OpenGL rending context to window: %s\n",
                        SDL_GetError());
    }

    enum Swap_Interval : s8 {
        immediate = 0,
        vertical_retrace_sync = 1,
        adaptive_vsync = -1
    };

    // Turn off VSync - it will be controlled from the renderer
    if (SDL_GL_SetSwapInterval(Swap_Interval::immediate) < 0) {
        SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM,
                     "SDL failed to set GL swap interval to immeidate, falling "
                     "back on veritcal retrace synchronization\n");
    } else if (SDL_GL_SetSwapInterval(Swap_Interval::vertical_retrace_sync) <
               0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                        "failed to set swap interval: %s\n", SDL_GetError());
    }
}

void Sdl2::swap_window_buffer() { SDL_GL_SwapWindow(window); }

void Sdl2::process_sys_event_queue()
{
    Game_State* state = &new_input->state;
    SDL_Event event;

    // Setup for new system input
    prepare_for_new_input();

    // Loop through waiting events
    // TODO(stewarts): Use SDL_WaitEvent to save power when the window is not
    // active.
    while (SDL_PollEvent(&event)) {
        // Handle event
        switch (event.type) {
            case SDL_QUIT: {
                state->request_quit = true;
            } break;

            case SDL_KEYDOWN: {
                process_keyboard_event(&event, true);
            } break;

            case SDL_KEYUP: {
                process_keyboard_event(&event, false);
            } break;

            case SDL_WINDOWEVENT: {
                process_window_event(&event.window);
            } break;

            case SDL_MOUSEBUTTONDOWN:
                // case SDL_MOUSEBUTTONUP:
                {
                    process_mouse_button_event(event);
                }
                break;

            case SDL_MOUSEMOTION: {
                process_mouse_motion_event(event);
            } break;

            default: {
            } break;
        }
    }
}

Game_Input* Sdl2::get_input() { return new_input; }

void Sdl2::prepare_for_new_input()
{
    // Save old input state
    std::swap(new_input, old_input);

    Game_State& new_game_state = new_input->state;
    Game_Input_Controller* new_keyboard =
        &new_input->controllers[Controller::keyboard];
    Game_Input_Controller* old_keyboard =
        &old_input->controllers[Controller::keyboard];

    // Reset toggles
    new_game_state.toggle_pause = false;

    // Zero the new keyboard
    *new_keyboard = zeroed_input.controllers[0];

    Game_Input_Button* new_button;
    Game_Input_Button* old_button;

    for (std::size_t button_index = 0;
         button_index <
         sizeof(new_keyboard->buttons) / sizeof(new_keyboard->buttons[0]);
         button_index++) {
        new_button = &new_keyboard->buttons[button_index];
        old_button = &old_keyboard->buttons[button_index];

        // We start down if we ended down on last input
        new_button->started_down = old_button->ended_down;

        // Since input has not been processed yet, there are no half transitions
        // ie. We ended down if we started down (updated during input
        // processing)
        new_button->ended_down = new_button->started_down;
    }
}

u32 Sdl2::get_ticks() { return SDL_GetTicks(); }

u64 Sdl2::get_performance_frequency() { return SDL_GetPerformanceFrequency(); }

u64 Sdl2::get_performance_counter() { return SDL_GetPerformanceCounter(); }

void Sdl2::process_window_event(SDL_WindowEvent* event)
{
    // TODO(sdsmith): port
    Game_State* state = &new_input->state;

    if (SDL_GetWindowID(window) == event->windowID) {
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
                // reshape(event->data1, window_event->data2);
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

void Sdl2::process_keyboard_event(SDL_Event* event, bool key_down)
{
    Game_Input_Controller& keyboard =
        new_input->controllers[Controller::keyboard];
    Game_State& game_state = new_input->state;
    const SDL_Keycode key = event->key.keysym.sym;

    switch (key) {
        case SDLK_w: {
            process_input_button(&keyboard.up, key_down);
        } break;

        case SDLK_a: {
            process_input_button(&keyboard.left, key_down);
        } break;

        case SDLK_s: {
            process_input_button(&keyboard.down, key_down);
        } break;

        case SDLK_d: {
            process_input_button(&keyboard.right, key_down);
        } break;

        case SDLK_p: {
            if (!key_down) { game_state.toggle_pause = true; }
        } break;

        case SDLK_ESCAPE: {
            if (!key_down) { game_state.request_quit = true; }
        } break;

        default: {
        } break;
    }
}

void Sdl2::process_input_button(Game_Input_Button* button, bool button_down)
{
    button->ended_down = button_down;
    button->half_transitions++;
}

// TODO(stewarts): seems like event.mouse.x
void Sdl2::process_mouse_button_event(const SDL_Event& event) const
{
    assert(event.type == SDL_MOUSEBUTTONDOWN ||
           event.type == SDL_MOUSEBUTTONUP);

    const SDL_MouseButtonEvent& button = event.button;

    // NOTE(stewarts): event.state => SDL_PRESSED, SDL_RELEASED
    // NOTE(stewarts): event.x, event.y

    const char* button_str = "unknown";
    switch (button.button) {
        case SDL_BUTTON_LEFT: {
            button_str = "left";
        } break;

        case SDL_BUTTON_MIDDLE: {
            button_str = "middle";
        } break;

        case SDL_BUTTON_RIGHT: {
            button_str = "right";
        } break;

        default: {
        } break;
    }

    printf("mouse button %s: (%d, %d)\n", button_str, button.x, button.y);
}

void Sdl2::process_mouse_motion_event(const SDL_Event& event) const
{
    assert(event.type == SDL_MOUSEMOTION);
}
