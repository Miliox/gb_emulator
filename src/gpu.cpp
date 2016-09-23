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
    window(nullptr), renderer(nullptr), texture(nullptr), clock(0), mmu(mmu) {

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
        window = SDL_CreateWindow(window_title.c_str(),
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
    int scanline  = static_cast<int>(mmu.hwio_ly);

    clear_scanline(scanline);

    if (mmu.hwio_lcdc & LCDC_FLAG_BACKGROUND_DISPLAY_ENABLE) {
        render_background_scanline(scanline);
    }

    if (mmu.hwio_lcdc & LCDC_FLAG_SPRITE_DISPLAY_ENABLE) {
        render_sprite_scanline(scanline);
    }
}
const Uint32 kShadePalette[4] = {SHADE_0, SHADE_1, SHADE_2, SHADE_3};

void GBGPU::clear_scanline(const int scanline) {
    int line_begin = scanline * SCREEN_WIDTH;
    int line_end = line_begin + SCREEN_WIDTH;
    std::fill(framebuffer.begin() + line_begin, framebuffer.begin() + line_end, SHADE_0);
}

void GBGPU::render_background_scanline(const int scanline) {
    int offset_line = mmu.hwio_scy;

    int line = (scanline + offset_line) % 256;

    uint16_t bg_addr = (mmu.hwio_lcdc & LCDC_FLAG_BACKGROUND_TILE_MAP_DISPLAY_SELECT)
        ? 0x9c00 : 0x9800;

    for (int column = 0; column < SCREEN_WIDTH; column++) {
        uint16_t bg_index = (line / kTileHeight) * kTilesPerRow;
        bg_index += (column / kTileWidth);

        uint16_t tile_number = mmu.read_byte(bg_addr + bg_index);

        uint16_t tile_addr = (mmu.hwio_lcdc & LCDC_FLAG_BACKGROUND_WINDOW_TILE_DATA_SELECT)
            ? 0x8000 : 0x9000;

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
    uint16_t render_line = scanline;

    bool is8x16 = mmu.hwio_lcdc & LCDC_FLAG_SPRITE_SIZE;
    const uint8_t kSpriteWidth = 8;
    const uint8_t kSpriteHeight = (is8x16) ? 16 : 8;

    std::vector<GBSprite*> visible_sprites;

    // sort visible sprites in the line
    GBSprite* sprite = reinterpret_cast<GBSprite*>(mmu.get_oam_ram_head());
    uint16_t sprite_count = mmu.get_oam_ram_size() / sizeof(GBSprite);
    for(int i = 0; i < sprite_count; i++) {

        if (sprite[i].x == 0 || sprite[i].x >= (160 + 8) ||
            sprite[i].y == 0 || sprite[i].y >= (144 + 16)) {
            continue;
        }

        if (sprite[i].screenY() > render_line || (sprite[i].screenY() + kSpriteHeight) < render_line) {
            continue;
        }

        visible_sprites.push_back(&sprite[i]);
    }

    const uint16_t kSpriteTileSize = 16;
    const uint16_t kSpriteTileLineSize = 2;
    const uint16_t kSpriteTileAddress = 0x8000;
    for (auto &sprite : visible_sprites) {
        uint16_t tileNumber = is8x16 ? (sprite->tile & 0xfe) : sprite->tile;
        uint16_t tileAddress = kSpriteTileAddress + (tileNumber * kSpriteTileSize);
        uint16_t tileLine = sprite->screenY() - render_line;

        if (sprite->is_yflipped()) {
            tileLine = (kSpriteHeight - 1) - tileLine;
        }

        uint16_t tileLineAddress = tileAddress + (tileLine * kSpriteTileLineSize);

        uint8_t lsb = mmu.read_byte(tileLineAddress);
        uint8_t msb = mmu.read_byte(tileLineAddress + 1);

        uint8_t sprite_pallete = sprite->is_pallet1() ? mmu.hwio_obp1 : mmu.hwio_obp0;

        for (int i = 0; i < kSpriteWidth; i++) {
            if ((sprite->x + i) < kSpriteWidth || (sprite->screenX() + i) >= SCREEN_WIDTH) {
                // Pixel not visible
                continue;
            }

            int bit_index = sprite->is_xflipped() ? i : (7 - i);

            int sprite_palette_index = 0;
            sprite_palette_index += ((lsb >> bit_index) & 0x01) ? 2 : 0;
            sprite_palette_index += ((msb >> bit_index) & 0x01) ? 1 : 0;

            int pallete_index = (sprite_pallete >> (sprite_palette_index * 2)) & 0x3;

            int column = sprite->screenX() + i;
            int pos = column + (scanline * SCREEN_WIDTH);
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
    if (mmu.hwio_lcdc & 0x80) {
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
}

void GBGPU::step(uint8_t elapsed_ticks) {
    clock += elapsed_ticks;

    GPUMode mode = static_cast<GPUMode>(mmu.hwio_stat & 0x3);

    switch (mode) {
        case HBLANK:
            if (clock >= 204) {
                clock -= 204;
                mmu.hwio_ly += 1;
                mmu.check_lcdc_line_coincidence();

                if (mmu.hwio_ly >= 143) {
                    mode = VBLANK;
                    mmu.request_interrupt(INTERRUPT_VBLANK);
                    mmu.request_lcdc_interrupt(LCDC_INTERRUPT_VBLANK);
                    refresh();
                } else {
                    mode = READOAM;
                    mmu.request_lcdc_interrupt(LCDC_INTERRUPT_OAM);
                }

            }
            break;
        case VBLANK:
            if (clock >= 456) {
                clock -= 456;
                mmu.hwio_ly += 1;

                if (mmu.hwio_ly > 153) {
                    mode = READOAM;
                    mmu.request_lcdc_interrupt(LCDC_INTERRUPT_OAM);
                    mmu.hwio_ly = 0;
                }

                mmu.check_lcdc_line_coincidence();
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
                mmu.request_lcdc_interrupt(LCDC_INTERRUPT_HBLANK);
                renderscan();
            }
            break;
    }

    mmu.hwio_stat = (mmu.hwio_stat & 0xfc) | (mode & 0x03);
}

void GBGPU::set_window_title(const std::string& title) {
    window_title = title;
    if (window) {
        SDL_SetWindowTitle(window, window_title.c_str());
    }
}
