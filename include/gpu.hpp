#ifndef GPU_HPP
#define GPU_HPP

#include <SDL.h>
#include <vector>

#include "mmu.hpp"
#include "sprite.hpp"

enum GPUMode : uint8_t {
    HBLANK  = 0,
    VBLANK  = 1,
    READOAM = 2,
    WRIVRAM = 3
};

enum LcdcFlags : uint8_t {
    LCDC_FLAG_DISPLAY_ENABLE                     = (1 << 7),
    LCDC_FLAG_WINDOW_TILE_MAP_DISPLAY_SELECT     = (1 << 6),
    LCDC_FLAG_WINDOW_DISPLAY_ENABLE              = (1 << 5),
    LCDC_FLAG_BACKGROUND_WINDOW_TILE_DATA_SELECT = (1 << 4),
    LCDC_FLAG_BACKGROUND_TILE_MAP_DISPLAY_SELECT = (1 << 3),
    LCDC_FLAG_SPRITE_SIZE                        = (1 << 2),
    LCDC_FLAG_SPRITE_DISPLAY_ENABLE              = (1 << 1),
    LCDC_FLAG_BACKGROUND_DISPLAY_ENABLE          = (1 << 0)
};

class GBGPU {
private:
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  texture;

    std::vector<Uint32> framebuffer;

    tick_t clock;

    std::string window_title;

    uint16_t decode_background_address(const uint8_t line, const uint8_t column);

    void render_background_scanline(const int scanline);
    void render_sprite_scanline(const int scanline);

public:
    GBMMU& mmu;

    GBGPU(GBMMU&);
    ~GBGPU();

    void show();
    void hide();

    void black();
    void blank();

    void renderscan();
    void refresh();

    void step(uint8_t elapsed_ticks);

    void set_window_title(const std::string&);
};

#endif
