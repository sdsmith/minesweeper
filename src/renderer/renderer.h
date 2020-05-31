#pragma once

#include "platform/platform.h"

class Renderer {
public:
    virtual ~Renderer() = default;
    ;
    virtual void clear_screen() = 0;
    virtual void swap_buffer() = 0;
    virtual void set_window_size(u32 w, u32 h) = 0;
    virtual void proto_setup() = 0;
    virtual void proto_draw() = 0;
};
