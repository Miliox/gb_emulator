#include "gpu.hpp"

#include <iostream>
#include <iomanip>
#include <string>

#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 144
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

#define TILE_SIZE 16

#define TILE_ROW  32
#define TILE_COL  32

#define TILE_HEIGHT 8

// WHITE
#define SHADE_0 0xFF9BBC0F
// LIGHT GRAY
#define SHADE_1 0xFF8BAC0F
// DARK GRAY
#define SHADE_2 0xFF306230
// BLACK
#define SHADE_3 0xFF0f380f

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
    const Uint32 palette[4] = {SHADE_0, SHADE_1, SHADE_2, SHADE_3};
    uint8_t scanline = mmu.hwio_ly - 1;

    for (int i = 0; i < SCREEN_WIDTH; i+= 8) {
        uint16_t addr = decode_background_address(scanline, i);

        uint8_t lsb = mmu.read_byte(addr);
        uint8_t msb = mmu.read_byte(addr + 1);

        for (int j = 0; j < 8; j++) {
            int bit_index = 7 - j;

            int background_palette_index = 0;
            background_palette_index += ((lsb >> bit_index) & 0x01) ? 2 : 0;
            background_palette_index += ((msb >> bit_index) & 0x01) ? 1 : 0;

            int pallete_index = (mmu.hwio_bgp >> (background_palette_index * 2)) & 0x3;

            int pos = (i + j) + (scanline * SCREEN_WIDTH);
            framebuffer[pos] = palette[pallete_index];
        }
    }

}

uint16_t GBGPU::decode_background_address(const uint8_t line, const uint8_t column) {
    uint16_t tile_addr = (mmu.hwio_lcdc & (1 << 3)) ? 0x9c00 : 0x9800;
    tile_addr += ((line / TILE_HEIGHT) * TILE_ROW);
    tile_addr += column / 8;

    uint8_t tile = mmu.read_byte(tile_addr);

    int addr = (mmu.hwio_lcdc & (1 << 4)) ? 0x8000 : 0x8800;
    if (addr == 0x8000 || tile < 128) {
        addr += tile * TILE_SIZE;
    } else {
        addr += static_cast<int8_t>(tile) * TILE_SIZE;
    }
    addr += 2 * (line % 8);

    return static_cast<uint16_t>(addr);
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
