#pragma once

#include "types.h"
#include <cstddef>

using Controller_ID = std::size_t;

enum Controller { keyboard = 0 };

struct Game_Input_Button {
    bool started_down;
    bool ended_down;
    s32 half_transitions;
};

struct Game_Input_Controller {
    union {
        Game_Input_Button buttons[4];
        struct {
            Game_Input_Button up;
            Game_Input_Button down;
            Game_Input_Button left;
            Game_Input_Button right;
        };
    };
};

struct Game_State {
    bool request_quit;
    bool toggle_pause;
};

struct Game_Input {
    Game_State state;
    Game_Input_Controller controllers[1];
};
