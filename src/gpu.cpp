#include "gpu.hpp"

#include <iostream>

#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 144

// WHITE
#define SHADE_0 0x9BBC0F
// LIGHT GRAY
#define SHADE_1 0x8BAC0F
// DARK GRAY
#define SHADE_2 0x306230
// BLACK
#define SHADE_3 0x0f380f

#define R(color) static_cast<Uint8>(color >> 16)
#define G(color) static_cast<Uint8>(color >> 8)
#define B(color) static_cast<Uint8>(color >> 0)

GBGPU::GBGPU(GBMMU& mmu) : window(nullptr), screen(nullptr), mmu(mmu) {

}

GBGPU::~GBGPU() {
    if (window) {
        SDL_DestroyWindow(window);
    }
}

void GBGPU::show() {
    if (!window) {
        window = SDL_CreateWindow("GBEmu",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN );

        if (window) {
            screen = SDL_GetWindowSurface(window);
            blank();
        } else {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        }
    }
}

void GBGPU::hide() {
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
        screen = nullptr;
    }
}

void GBGPU::blank() {
    if (!screen || !window) {
        return;
    }

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, R(SHADE_0), G(SHADE_0), B(SHADE_0)));
    SDL_UpdateWindowSurface(window);
}

void GBGPU::refresh() {
    //TODO: To be implemented!
}
