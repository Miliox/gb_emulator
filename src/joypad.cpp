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

    pressed_keys |= start  ? 0 : (1 << 7);
    pressed_keys |= select ? 0 : (1 << 6);
    pressed_keys |= b      ? 0 : (1 << 5);
    pressed_keys |= a      ? 0 : (1 << 4);
    pressed_keys |= down   ? 0 : (1 << 3);
    pressed_keys |= up     ? 0 : (1 << 2);
    pressed_keys |= left   ? 0 : (1 << 1);
    pressed_keys |= right  ? 0 : (1 << 0);

    return pressed_keys;
}
