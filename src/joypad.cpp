#include "joypad.hpp"

GBJoypad::GBJoypad() :
    up(false), left(false), right(false), down(false),
    a(false), b(false), start(false), select(false) {

}

void GBJoypad::process_events(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        bool pressed = event.type == SDL_KEYDOWN;
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                up = pressed;
                break;
            case SDLK_LEFT:
                left = pressed;
                break;
            case SDLK_RIGHT:
                right = pressed;
                break;
            case SDLK_DOWN:
                down = pressed;
                break;
            case SDLK_z:
                b = pressed;
                break;
            case SDLK_x:
                a = pressed;
                break;
            case SDLK_a:
                select = pressed;
                break;
            case SDLK_s:
                start = pressed;
                break;
        }
    }
}

uint8_t GBJoypad::get_pressed_keys() {
    uint8_t pressed_keys = 0;

    pressed_keys |= start  ? (1 << 7) : 0;
    pressed_keys |= select ? (1 << 6) : 0;
    pressed_keys |= b      ? (1 << 5) : 0;
    pressed_keys |= a      ? (1 << 4) : 0;
    pressed_keys |= down   ? (1 << 3) : 0;
    pressed_keys |= up     ? (1 << 2) : 0;
    pressed_keys |= left   ? (1 << 1) : 0;
    pressed_keys |= right  ? (1 << 0) : 0;

    return pressed_keys;
}
