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
    virtual u32 getTicks(void) = 0;
    virtual u64 getPerformanceFrequency(void) = 0;
    virtual u64 getPerformanceCounter(void) = 0;
    // virtual void *platform_alloc(size_t size) = 0;
    // virtual void platform_free(void *p) = 0;
    virtual void processSysEventQueue(void) = 0;
    virtual Game_Input* getInput(void) = 0;
    virtual void createOpenGlRenderingContext(s32 glMajorVersion, s32 glMinorVersion,
                                              s32 windowWidth, s32 windowHeight) = 0;
    virtual void swapWindowBuffer(void) = 0;
    virtual void setWindowSize(u32 w, u32 h) = 0;
};
