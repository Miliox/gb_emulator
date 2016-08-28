#include "gpu.hpp"

#include <iostream>

#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 144

GBGPU::GBGPU(GBMMU& mmu) : display(nullptr), mmu(mmu) {

}

GBGPU::~GBGPU() {
    if (display) {
        SDL_DestroyWindow(display);
    }
    display = nullptr;
}

void GBGPU::show() {
    if (!display) {
        display = SDL_CreateWindow("GBEmu",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN );

        if (!display) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        }
    }
}

void GBGPU::hide() {
    if (display) {
        SDL_DestroyWindow(display);
        display = nullptr;
    }
}
