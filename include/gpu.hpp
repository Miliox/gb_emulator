#ifndef GPU_HPP
#define GPU_HPP

#include <SDL.h>

#include "mmu.hpp"

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

    Uint32* framebuffer;

    GPUMode   mode;
    uint16_t clock;

    bool is_on;

    void check_enable_changed();
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
};

#endif
