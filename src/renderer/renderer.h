#pragma once

#include "platform/platform.h"

class Renderer {
public:
    virtual ~Renderer() {};
    virtual void clearScreen() = 0;
    virtual void swapBuffer() = 0;
    virtual void setWindowSize(u32 w, u32 h) = 0;
    virtual void protoSetup() = 0;
    virtual void protoDraw() = 0;
};
