#pragma once

#include "input.h"
#include "types.h"

/**
   Platform interface.

   TODO(sdsmith): determine exactly what needs to be exposed beyond the platform
   layer.
 */
class Platform {
public:
    virtual ~Platform(void) {};
    virtual u32 get_ticks(void) = 0;
    virtual u64 get_performance_frequency(void) = 0;
    virtual u64 get_performance_counter(void) = 0;
    // virtual void *platform_alloc(size_t size) = 0;
    // virtual void platform_free(void *p) = 0;
    virtual void process_sys_event_queue(void) = 0;
    virtual Game_Input* get_input(void) = 0;
    virtual void create_open_gl_rendering_context(s32 gl_major_version, s32 gl_minor_version,
                                              s32 window_width, s32 window_height) = 0;
    virtual void swap_window_buffer(void) = 0;
    virtual void set_window_size(u32 w, u32 h) = 0;
};
