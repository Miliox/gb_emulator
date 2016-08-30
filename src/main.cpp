#include "main.hpp"

#include <SDL.h>

void dump_inst(uint8_t, const GBCPU&);
void dump_cpu(const GBCPU&);

void emulator() {
    GBMMU mmu;
    GBCPU cpu(mmu);
    GBGPU gpu(mmu);

    bool running = true;

    gpu.show();
    try {
        while(running) {
            // fetch
            uint8_t op = cpu.mmu.read_byte(cpu.reg.pc++);
            //dump_inst(op, cpu);

            // decode
            auto& instruction = cpu.instruction_map.at(op);

            // execute
            tick_t t = (cpu.*instruction)();
            //dump_cpu(cpu);
            //std::cout << "\n";

            gpu.step(t);
            mmu.step(t);

            // check for quit interruption
            SDL_Event event;
            if (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                    running = false;
                }
            }

            //SDL_Delay(1);
        }
    } catch (std::exception& e) {
        std::cout << "error: " << e.what() << "\n";
    }
    gpu.hide();
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) >= 0) {
        emulator();
        SDL_Quit();
    } else {
        std::cerr << "Window could not be created! SDL_Error:" << SDL_GetError() << "\n";
    }
    return 0;
}

void dump_inst(uint8_t opcode, const GBCPU& cpu) {
    std::cout << std::hex;
    if (opcode != 0xcb) {
        std::cout << kGBCPUInstrunctionNames[opcode];

        std::cout << " : " << static_cast<uint16_t>(opcode);
        for (int i = 1; i < kInstrunctionLength[opcode]; i++) {
            std::cout << " " << static_cast<uint16_t>(cpu.mmu.read_byte(cpu.reg.pc + i - 1));
        }

    } else {
        std::cout << "op:" << static_cast<uint16_t>(opcode);
        std::cout << " " << static_cast<uint16_t>(cpu.mmu.read_byte(cpu.reg.pc));
    }
    std::cout << std::dec << std::endl;
}

void dump_cpu(const GBCPU& cpu) {
    std::cout << std::hex;
    std::cout << "a:" << static_cast<uint16_t>(cpu.reg.a) << ", ";
    std::cout << "f:" << static_cast<uint16_t>(cpu.reg.f) << ", ";
    std::cout << "b:" << static_cast<uint16_t>(cpu.reg.b) << ", ";
    std::cout << "c:" << static_cast<uint16_t>(cpu.reg.c) << ", ";
    std::cout << "d:" << static_cast<uint16_t>(cpu.reg.d) << ", ";
    std::cout << "e:" << static_cast<uint16_t>(cpu.reg.e) << ", ";
    std::cout << "h:" << static_cast<uint16_t>(cpu.reg.h) << ", ";
    std::cout << "l:" << static_cast<uint16_t>(cpu.reg.l) << ", ";

    std::cout << "sp: " << cpu.reg.sp << ", ";
    std::cout << "pc: " << cpu.reg.pc;
    std::cout << std::dec;
}
