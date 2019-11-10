#pragma once

#include "platform.h"
#include "input.h"
#include "types.h"
#include <SDL2/SDL.h>

class Sdl2 : public Platform {
public:
    Sdl2();
    Sdl2(const Sdl2& o) = delete;
    ~Sdl2();

    void create_open_gl_rendering_context(s32 gl_major_version, s32 gl_minor_version,
                                      s32 window_width, s32 window_height);
    void swap_window_buffer(void);
    void process_sys_event_queue(void);
    Game_Input* get_input(void);
    u32 get_ticks(void);
    u64 get_performance_frequency(void);
    u64 get_performance_counter(void);
    void set_window_size(u32 w, u32 h);

    Sdl2& operator=(const Sdl2& o) = delete;

private:
    SDL_Window* window;
    SDL_GLContext gl_context;

    Game_Input input[2]; // TODO(sdsmith): confirm 0-initialization via the constructor initialization list
    Game_Input* new_input;
    Game_Input* old_input;
    static const Game_Input zeroed_input; // TODO(sdsmith): confirm 0-initializaton

    void prepare_for_new_input(void);
    void process_window_event(SDL_WindowEvent* event);
    void process_keyboard_event(SDL_Event* event, bool key_down);
    void process_input_button(Game_Input_Button* button, bool button_down);
};
