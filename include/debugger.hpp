#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <sstream>
#include <queue>

#include "cpu.hpp"
#include "gpu.hpp"
#include "mmu.hpp"
#include "joypad.hpp"
#include "instruction.hpp"

class Debugger {
private:
    GBCPU& cpu;
    GBGPU& gpu;
    GBMMU& mmu;
    GBJoypad& joypad;

    SDL_Window*   window;
    SDL_Renderer* renderer;
    TTF_Font*     font;

    std::queue<Instruction> last_cpu_instructions;

    std::vector<std::string> dump_registers();
    std::vector<std::string> dump_memory();
    std::vector<std::string> dump_executed_instructions();

public:
    Debugger(GBCPU& cpu, GBGPU& gpu, GBJoypad& joypad);
    void show();
    void hide();
    void draw();

    void log_instruction();
};

#endif
