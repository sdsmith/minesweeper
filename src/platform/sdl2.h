#pragma once

#include "input.h"
#include "platform.h"
#include "types.h"
#include <SDL2/SDL.h>

class Sdl2 : public Platform {
public:
    Sdl2();
    Sdl2(const Sdl2& o) = delete;
    ~Sdl2() override;

    void create_open_gl_rendering_context(char const* window_name,
                                          s32 gl_major_version,
                                          s32 gl_minor_version,
                                          s32 window_width,
                                          s32 window_height) override;
    void swap_window_buffer() override;
    void process_sys_event_queue() override;
    Game_Input* get_input() override;
    u32 get_ticks() override;
    u64 get_performance_frequency() override;
    u64 get_performance_counter() override;
    void set_window_size(u32 w, u32 h) override;

    Sdl2& operator=(const Sdl2& o) = delete;

private:
    SDL_Window* window{nullptr};
    SDL_GLContext gl_context{nullptr};

    Game_Input input[2]; // TODO(sdsmith): confirm 0-initialization via the
                         // constructor initialization list
    Game_Input* new_input;
    Game_Input* old_input;
    static const Game_Input
        zeroed_input; // TODO(sdsmith): confirm 0-initializaton

    void prepare_for_new_input();
    void process_window_event(SDL_WindowEvent* event);
    void process_keyboard_event(SDL_Event* event, bool key_down);
    void process_input_button(Game_Input_Button* button, bool button_down);
    void process_mouse_button_event(const SDL_Event& event) const;
    void process_mouse_motion_event(const SDL_Event& event) const;
};
