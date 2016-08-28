#include "gpu.hpp"

#include <iostream>
#include <iomanip>
#include <string>

#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 144
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

#define TILE_SIZE 16

// WHITE
#define SHADE_0 0xFF9BBC0F
// LIGHT GRAY
#define SHADE_1 0xFF8BAC0F
// DARK GRAY
#define SHADE_2 0xFF306230
// BLACK
#define SHADE_3 0xFF0f380f

const Uint32 kLCDPalette[4] = {SHADE_0, SHADE_1, SHADE_2, SHADE_3};

#define R(color) static_cast<Uint8>(color >> 16)
#define G(color) static_cast<Uint8>(color >> 8)
#define B(color) static_cast<Uint8>(color >> 0)

GBGPU::GBGPU(GBMMU& mmu) :
    window(nullptr), renderer(nullptr), texture(nullptr), framebuffer(nullptr),
    mode(HBLANK), clock(0),  is_on(false), mmu(mmu) {

}

GBGPU::~GBGPU() {
    if (framebuffer) {
        delete[] framebuffer;
    }

    if (texture) {
        SDL_DestroyTexture(texture);
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

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
            renderer = SDL_CreateRenderer(window, -1, 0);
            texture  = SDL_CreateTexture(renderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                SCREEN_WIDTH, SCREEN_HEIGHT);
            framebuffer = new Uint32[SCREEN_SIZE];
            for (int i = 0; i < SCREEN_SIZE; i++) {
                framebuffer[i] = SHADE_0;
            }
            black();
        } else {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        }
    }
}

void GBGPU::hide() {
    if (framebuffer) {
        delete[] framebuffer;
        framebuffer = nullptr;
    }

    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void GBGPU::blank() {
    if (!texture || !renderer || !window) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, R(SHADE_0), G(SHADE_0), B(SHADE_0), 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void GBGPU::black() {
    if (!texture || !renderer || !window) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, R(SHADE_3), G(SHADE_3), B(SHADE_3), 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void GBGPU::renderscan() {
    uint16_t wnbase_addr = (mmu.hwio_lcdc & (1 << 6)) ? 0x9c00 : 0x9800;
    uint16_t bgbase_addr = (mmu.hwio_lcdc & (1 << 3)) ? 0x9c00 : 0x9800;
    uint16_t tlbase_addr = (mmu.hwio_lcdc & (1 << 4)) ? 0x8000 : 0x8800;

    uint16_t sprite_width = (mmu.hwio_lcdc & (1 << 2)) ? 16 : 8;

    // uint8_t lin = mmu.hwio_ly + mmu.hwio_scy;
    // uint8_t col = mmu.hwio_scx;

    uint8_t lin = mmu.hwio_ly - 1;
    uint8_t col = 0;

    uint8_t dy = lin / 8;
    uint8_t ry = lin % 8;

    uint16_t map_addr = bgbase_addr + (dy * 32);

    uint8_t tile = mmu.read_byte(map_addr);

    uint8_t palette[4];

    uint16_t bg_addr = tlbase_addr;
    if (bg_addr == 0x8000) {
        bg_addr += tile * TILE_SIZE;
    } else {
        int32_t aux(bg_addr);
        aux += static_cast<int16_t>(tile) * TILE_SIZE;
        bg_addr = static_cast<uint16_t>(aux);
    }
    bg_addr += 2 * ry;

    uint8_t bg[2];
    bg[1] = mmu.read_byte(bg_addr);
    bg[0] = mmu.read_byte(bg_addr + 1);

    for (int i = 0; i < SCREEN_WIDTH; i++) {
        int pos = i + lin * SCREEN_WIDTH;

        int index = 7 - (i % 8);

        int color = 0;
        color += ((bg[0] >> index) & 0x01) ? 2 : 0;
        color += ((bg[1] >> index) & 0x01) ? 1 : 0;

        color = (mmu.hwio_bgp >> (color * 2)) & 0x3;

        switch (color) {
            case 0:
                framebuffer[pos] = SHADE_0;
                break;
            case 1:
                framebuffer[pos] = SHADE_1;
                break;
            case 2:
                framebuffer[pos] = SHADE_2;
                break;
            case 3:
                framebuffer[pos] = SHADE_3;
                break;
        }

        if (i > 0 && (i % 8) == 0) {
            map_addr += 1;
            tile = mmu.read_byte(map_addr);

            bg_addr = tlbase_addr;
            if (bg_addr == 0x8000) {
                bg_addr += tile * TILE_SIZE;
            } else {
                int32_t aux(bg_addr);
                aux += static_cast<int16_t>(tile) * TILE_SIZE;
                bg_addr = static_cast<uint16_t>(aux);
            }
            bg_addr += 2 * ry;

            bg[1] = mmu.read_byte(bg_addr);
            bg[0] = mmu.read_byte(bg_addr + 1);
        }
    }

}

void GBGPU::refresh() {
    int pitch = 0;
    Uint32* pixels = nullptr;
    SDL_LockTexture(texture, nullptr,
        reinterpret_cast<void **>(&pixels), &pitch);

    memcpy(pixels, framebuffer, SCREEN_SIZE * sizeof(Uint32));

    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
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
                    renderscan();
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
                refresh();
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
