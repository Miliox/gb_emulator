#include "gpu.hpp"

#include <iostream>
#include <iomanip>
#include <string>

#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 144
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

const uint16_t kTileSize = 16;
const uint16_t kTileWidth = 8;
const uint16_t kTileHeight = 8;
const uint16_t kTilesPerRow = 32;
const uint16_t kTilesPerColumn = 32;

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
    window(nullptr), renderer(nullptr), texture(nullptr),
    mode(HBLANK), clock(0),  is_on(false), mmu(mmu) {

}

GBGPU::~GBGPU() {
    framebuffer.clear();

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
            framebuffer = std::vector<Uint32>(SCREEN_SIZE, SHADE_0);
            black();
        } else {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        }
    }
}

void GBGPU::hide() {
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
    int scanline  = static_cast<int>(mmu.hwio_ly) - 1;
    if (mmu.hwio_lcdc & 0x01) {
        render_background_scanline(scanline);
    }

    if (mmu.hwio_lcdc & 0x02) {
        render_sprite_scanline(scanline);
    }
}
const Uint32 kShadePalette[4] = {SHADE_0, SHADE_1, SHADE_2, SHADE_3};

void GBGPU::render_background_scanline(const int scanline) {
    int offset_line = mmu.hwio_scy;

    int line = (scanline + offset_line) % 256;

    uint16_t bg_addr = (mmu.hwio_lcdc & 0x08) ? 0x9c00 : 0x9800;

    for (int column = 0; column < SCREEN_WIDTH; column++) {
        uint16_t bg_index = (line / kTileHeight) * kTilesPerRow;
        bg_index += (column / kTileWidth);

        uint16_t tile_number = mmu.read_byte(bg_addr + bg_index);

        uint16_t tile_addr = (mmu.hwio_lcdc & 0x10) ? 0x9000 : 0x8000;
        if (tile_addr == 0x9000 && tile_number >= 128) {
            tile_number = (0xff - tile_number) + 1;
            tile_addr -= (tile_number * 16);
        } else {
            tile_addr += (tile_number * 16);
        }
        tile_addr += (line % kTileHeight) * 2;

        uint8_t lsb = mmu.read_byte(tile_addr + 0);
        uint8_t msb = mmu.read_byte(tile_addr + 1);

        int bit_index = 7 - (column % 8);

        int pallete_index = 0;
        pallete_index += ((lsb >> bit_index) & 0x01) ? 2 : 0;
        pallete_index += ((msb >> bit_index) & 0x01) ? 1 : 0;

        pallete_index = (mmu.hwio_bgp >> (pallete_index * 2)) & 0x3;

        int pos = column + (scanline * SCREEN_WIDTH);
        framebuffer.at(pos) = kShadePalette[pallete_index];
    }
}

const uint16_t kSizeSprite = 4;

void GBGPU::render_sprite_scanline(const int scanline) {
    uint8_t sprite_width  = 8;
    uint8_t sprite_height = (mmu.hwio_lcdc & 0x04) ? 16 : 8;

    std::vector<GBSprite> sprites;
    for (int i = 0; i < 40; i++) {
        uint16_t sprite_addr = 0xfe00 + i * kSizeSprite;

        uint8_t y = mmu.read_byte(sprite_addr) - 16;
        uint8_t x = mmu.read_byte(sprite_addr + 1) - 8;

        if (y == 0 || y >= 160) {
            continue;
        }

        if (x == 0 || x >= 168) {
            continue;
        }

        if (scanline >= y &&  scanline < (y + sprite_height)) {
            uint8_t tile = mmu.read_byte(sprite_addr + 2);
            uint8_t flags = mmu.read_byte(sprite_addr + 3);
            sprites.push_back(GBSprite(x, y, tile, flags));
        }
    }

    uint16_t sprite_base_addr = 0x8000;

    for (auto& sprite : sprites) {
        int line = scanline - sprite.y;
        if (sprite.y_flip) {
            line -= sprite_height;
            line *= -1;
        }
        line *= 2;

        uint16_t tile_addr = sprite_base_addr + line + (sprite.tile * 16);

        uint8_t lsb = mmu.read_byte(tile_addr);
        uint8_t msb = mmu.read_byte(tile_addr + 1);

        uint8_t sprite_pallete = sprite.is_pallete_1 ? mmu.hwio_obp1 : mmu.hwio_obp0;

        for (int i = 0; i < sprite_width; i++) {
            int bit_index = 7 - i;

            int sprite_palette_index = 0;
            sprite_palette_index += ((lsb >> bit_index) & 0x01) ? 2 : 0;
            sprite_palette_index += ((msb >> bit_index) & 0x01) ? 1 : 0;

            int pallete_index = (sprite_pallete >> (sprite_palette_index * 2)) & 0x3;

            int pos = (sprite.x + i) + (scanline * SCREEN_WIDTH);
            framebuffer.at(pos) = kShadePalette[pallete_index];
        }
    }
}

uint16_t GBGPU::decode_background_address(const uint8_t line, const uint8_t column) {
    uint16_t tile_addr = (mmu.hwio_lcdc & (1 << 3)) ? 0x9c00 : 0x9800;
    tile_addr += ((line / kTileHeight) * kTilesPerRow);
    tile_addr += column / 8;

    uint8_t tile = mmu.read_byte(tile_addr);

    int addr = (mmu.hwio_lcdc & (1 << 4)) ? 0x8000 : 0x8800;
    if (addr == 0x8000 || tile < 128) {
        addr += tile * kTileSize;
    } else {
        addr += static_cast<int8_t>(tile) * kTileSize;
    }
    addr += 2 * (line % 8);

    return static_cast<uint16_t>(addr);
}

void GBGPU::refresh() {
    int pitch = 0;
    Uint32* pixels = nullptr;
    SDL_LockTexture(texture, nullptr,
        reinterpret_cast<void **>(&pixels), &pitch);

    std::copy(framebuffer.begin(), framebuffer.end(), pixels);

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

                    mmu.hwio_if |= kInterruptionVBlank;
                    if (mmu.hwio_stat & kLcdInterruptVBlank) {
                        mmu.hwio_if |= kInterruptionLcdStat;
                    }

                } else {
                    mode = READOAM;

                    if (mmu.hwio_stat & kLcdInterruptOAM) {
                        mmu.hwio_if |= kInterruptionLcdStat;
                    }

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

                    if (mmu.hwio_stat & kLcdInterruptOAM) {
                        mmu.hwio_if |= kInterruptionLcdStat;
                    }

                    mmu.hwio_ly = 0;
                    refresh();
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

                if (mmu.hwio_stat & kLcdInterruptHBlank) {
                    mmu.hwio_if |= kInterruptionLcdStat;
                }

            }
            break;
    }

    if (mmu.hwio_ly == mmu.hwio_lyc) {
        mmu.hwio_stat |= 0x40;

        if (mmu.hwio_stat & kLcdInterruptLineEq) {
            mmu.hwio_if |= kInterruptionLcdStat;
        }
    }
    mmu.hwio_stat = (mmu.hwio_stat & 0xfc) | (mode & 0x03);
}

void GBGPU::check_enable_changed() {
    bool enable = (mmu.hwio_lcdc & 0x80) != 0;

    if (enable && !is_on) {
        blank();
        clock = 0;
        is_on = true;
    } else if (!enable && is_on) {

        clock = 0;
        is_on = false;
    }
}
