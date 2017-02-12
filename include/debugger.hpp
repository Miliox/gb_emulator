#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <SDL.h>
#include <SDL_ttf.h>

#include "cpu.hpp"
#include "gpu.hpp"
#include "mmu.hpp"
#include "joypad.hpp"

class Debugger {
private:
    GBCPU& cpu;
    GBGPU& gpu;
    GBMMU& mmu;
    GBJoypad& joypad;

    SDL_Window*   window;
    SDL_Renderer* renderer;
    TTF_Font*     font;

    std::vector<std::string> dump_registers();

public:
    Debugger(GBCPU& cpu, GBGPU& gpu, GBJoypad& joypad);
    void show();
    void hide();
    void draw();
};

#endif
