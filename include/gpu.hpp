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

class GBGPU {
private:
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  texture;

    std::vector<Uint32> framebuffer;

    uint16_t clock;

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
