#ifndef GPU_HPP
#define GPU_HPP

#include <SDL.h>

#include "mmu.hpp"

class GBGPU {
private:
    SDL_Window* display;

public:
    GBGPU(GBMMU&);
    ~GBGPU();

    GBMMU& mmu;

    void show();
    void hide();
};

#endif
