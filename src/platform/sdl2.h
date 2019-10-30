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

    void createOpenGlRenderingContext(s32 glMajorVersion, s32 glMinorVersion,
                                      s32 windowWidth, s32 windowHeight);
    void swapWindowBuffer(void);
    void processSysEventQueue(void);
    Game_Input* getInput(void);
    u32 getTicks(void);
    u64 getPerformanceFrequency(void);
    u64 getPerformanceCounter(void);
    void setWindowSize(u32 w, u32 h);

    Sdl2& operator=(const Sdl2& o) = delete;

private:
    SDL_Window* window;
    SDL_GLContext glContext;

    Game_Input input[2]; // TODO(sdsmith): confirm 0-initialization via the constructor initialization list
    Game_Input* newInput;
    Game_Input* oldInput;
    static const Game_Input zeroedInput; // TODO(sdsmith): confirm 0-initializaton

    void prepareForNewInput(void);
    void processWindowEvent(SDL_WindowEvent* event);
    void processKeyboardEvent(SDL_Event* event, bool keyDown);
    void processInputButton(Game_Input_Button* button, bool buttonDown);
};
