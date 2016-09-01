#include "main.hpp"

#include <SDL.h>

void dump_inst(uint8_t, const GBCPU&);
void dump_cpu(const GBCPU&);
void unload_bios(GBCPU& cpu, GBMMU& mmu);
void process_events(bool& running);

void emulator() {
    GBMMU mmu;
    GBCPU cpu(mmu);
    GBGPU gpu(mmu);

    bool running = true;

    // skip bios checking
    unload_bios(cpu, mmu);

    gpu.show();
    try {
        tick_t clock = 0;
        while(running) {
            // fetch
            uint8_t op = cpu.mmu.read_byte(cpu.reg.pc++);
            //dump_inst(op, cpu);

            // decode
            auto& instruction = cpu.instruction_map.at(op);

            // execute
            tick_t t = (cpu.*instruction)();
            //dump_cpu(cpu);

            gpu.step(t);
            mmu.step(t);
            clock += t;

            // interrupt handler
            if (cpu.ime && (mmu.hwio_ie & mmu.hwio_if)) {
                cpu.ime = false;

                if (mmu.hwio_if & kInterruptionVBlank) {
                    mmu.hwio_if &= ~kInterruptionVBlank;
                    cpu.rst(0x40);
                    //std::cout << "catch vblank interruption\n";
                } else if (mmu.hwio_if & kInterruptionLcdStat) {
                    mmu.hwio_if &= ~kInterruptionLcdStat;
                    cpu.rst(0x48);
                    //std::cout << "catch lcdc stat interruption\n";
                } else if (mmu.hwio_if & kInterruptionTimer) {
                    mmu.hwio_if &= ~kInterruptionTimer;
                    cpu.rst(0x50);
                    //std::cout << "catch serial interruption\n";
                } else if (mmu.hwio_if & kInterruptionSerial) {
                    mmu.hwio_if &= ~kInterruptionSerial;
                    cpu.rst(0x58);
                    //std::cout << "catch serial interruption\n";
                } else if (mmu.hwio_if & kInterruptionJoypad) {
                    mmu.hwio_if &= ~kInterruptionJoypad;
                    cpu.rst(0x60);
                    //std::cout << "catch joypad interruption\n";
                }
            }

            // sync
            if (clock >= kTicksPerFrame) {
                clock -= kTicksPerFrame;

                process_events(running);
                SDL_Delay(kMillisPerFrame);
            }
        }
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
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
    std::cout << std::dec << "\n";
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
    std::cout << std::dec << "\n\n";
}

void unload_bios(GBCPU& cpu, GBMMU& mmu) {
    cpu.reg.af = 0x00b0;
    cpu.reg.bc = 0x0013;
    cpu.reg.de = 0x00d8;
    cpu.reg.hl = 0x014d;
    cpu.reg.sp = 0xfffe;
    cpu.reg.pc = 0x0100;

    mmu.bios_loaded = false;

    mmu.hwio_tima = 0x00;
    mmu.hwio_tma  = 0x00;
    mmu.hwio_tac  = 0x00;

    mmu.hwio_nr10 = 0x80;
    mmu.hwio_nr11 = 0xbf;
    mmu.hwio_nr12 = 0xf3;
    mmu.hwio_nr14 = 0xbf;

    mmu.hwio_nr21 = 0x3f;
    mmu.hwio_nr22 = 0x00;
    mmu.hwio_nr24 = 0xbf;

    mmu.hwio_nr30 = 0x7f;
    mmu.hwio_nr31 = 0xff;
    mmu.hwio_nr32 = 0x9f;
    mmu.hwio_nr33 = 0xbf;

    mmu.hwio_nr41 = 0xff;
    mmu.hwio_nr42 = 0x00;
    mmu.hwio_nr43 = 0x00;
    mmu.hwio_nr44 = 0xbf;

    mmu.hwio_nr50 = 0x77;
    mmu.hwio_nr51 = 0xf3;
    mmu.hwio_nr52 = 0xf1;

    mmu.hwio_lcdc = 0x91;
    mmu.hwio_scy  = 0x00;
    mmu.hwio_scx  = 0x00;
    mmu.hwio_lyc  = 0x00;
    mmu.hwio_bgp  = 0xfc;
    mmu.hwio_obp0 = 0xff;
    mmu.hwio_obp1 = 0xff;
    mmu.hwio_wx   = 0x00;
    mmu.hwio_wy   = 0x00;
    mmu.hwio_ie   = 0x00;
}

void process_events(bool& running) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            running = false;
        }
    }
}
