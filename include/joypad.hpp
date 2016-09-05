#ifndef JOYPAD_HPP
#define JOYPAD_HPP

#include <SDL.h>

#include "mmu.hpp"

class GBJoypad {
private:
    bool up;
    bool left;
    bool right;
    bool down;

    bool a;
    bool b;

    bool start;
    bool select;

public:
    GBJoypad();

    void process_events(const SDL_Event& event);

    uint8_t get_pressed_keys();
};

#endif
