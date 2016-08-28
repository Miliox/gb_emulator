#ifndef GPU_HPP
#define GPU_HPP

#include <SDL.h>

#include "mmu.hpp"

class GBGPU {
private:
    SDL_Window*  window;
    SDL_Surface* screen;

public:
    GBGPU(GBMMU&);
    ~GBGPU();

    GBMMU& mmu;

    void show();
    void hide();

    void blank();
    void refresh();

    void step(uint16_t elapsed_ticks);
};

#endif
