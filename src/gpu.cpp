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

GBGPU::GBGPU(GBMMU& mmu) :
    window(nullptr), screen(nullptr),
    mode(HBLANK), clock(0),  is_on(false), mmu(mmu) {

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
            black();
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

    SDL_FillRect(screen, NULL,
        SDL_MapRGB(screen->format, R(SHADE_0), G(SHADE_0), B(SHADE_0)));
    SDL_UpdateWindowSurface(window);
}

void GBGPU::black() {
    if (!screen || !window) {
        return;
    }

    SDL_FillRect(screen, NULL,
        SDL_MapRGB(screen->format, R(SHADE_3), G(SHADE_3), B(SHADE_3)));
    SDL_UpdateWindowSurface(window);
}

void GBGPU::refresh() {
    //TODO: To be implemented!
}

void GBGPU::step(uint8_t elapsed_ticks) {
    check_enable_changed();
    if (!is_on) {
        return;
    }

    clock += elapsed_ticks;
    switch (mode) {
        case HBLANK:
            if (clock >= 204) {
                clock -= 204;
                mmu.hwio_ly += 1;
                if (mmu.hwio_ly >= 143) {
                    mode = VBLANK;
                } else {
                    mode = READOAM;
                }
            }
            break;
        case VBLANK:
            if (clock >= 456) {
                clock -= 456;
                mmu.hwio_ly += 1;
                if (mmu.hwio_ly > 153) {
                    mode = READOAM;
                    mmu.hwio_ly = 0;
                }
            }
            break;
        case READOAM:
            if (clock >= 80) {
                clock -= 80;
                mode = WRIVRAM;
            }
            break;
        case WRIVRAM:
            if (clock >= 172) {
                clock -= 172;
                mode = HBLANK;
                //TODO: Render Frame
            }
            break;
    }

    if (mmu.hwio_ly == mmu.hwio_lyc) {
        mmu.hwio_stat |= 0x40;
    }
    mmu.hwio_stat = (mmu.hwio_stat & 0xfc) | (mode);
}

void GBGPU::check_enable_changed() {
    bool enable = (mmu.hwio_lcdc & 0x80) != 0;

    if (enable && !is_on) {
        blank();
        clock = 0;
        is_on = true;
    } else if (!enable && is_on) {
        black();
        clock = 0;
        is_on = false;
    }
}
