#include "main.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

void dump_inst(uint8_t, const GBCPU&);
void dump_cpu(const GBCPU&);
void unload_bios(GBCPU& cpu, GBMMU& mmu);
void process_events(bool& running, GBJoypad& joypad);

void emulator(const char* filename) {
    std::unique_ptr<GBCartridge> cartridge(new GBCartridge());
    cartridge->load(filename);
    if (!cartridge->is_loaded()) {
        return;
    }

    std::string game_title = !cartridge->title.empty() ? cartridge->title : "GBEmu";

    GBMMU mmu(cartridge); // ownership of cartridge transfered, don't use!
    GBCPU cpu(mmu);
    GBGPU gpu(mmu);
    GBJoypad joypad;

    Debugger debugger(cpu, gpu, joypad);

    gpu.set_window_title(game_title);

    bool running = true;

    // skip bios checking
    unload_bios(cpu, mmu);

    debugger.show();
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

            // sync
            if (clock >= kTicksPerFrame) {
                clock -= kTicksPerFrame;

                process_events(running, joypad);
                mmu.set_joypad_state(joypad.get_pressed_keys());

                debugger.draw();

                SDL_Delay(kMillisPerFrame);
            }

            // interrupt handler
            if (mmu.interrupt_master_enabled && (mmu.hwio_ie & mmu.hwio_if)) {
                mmu.disable_interrupts();

                t = 0;
                if (mmu.hwio_if & kInterruptionVBlank) {
                    mmu.hwio_if &= ~kInterruptionVBlank;
                    t += cpu.rst_40();
                    //std::cout << "catch vblank interruption\n";
                } else if (mmu.hwio_if & kInterruptionLcdStat) {
                    mmu.hwio_if &= ~kInterruptionLcdStat;
                    t += cpu.rst_48();
                    //std::cout << "catch lcdc stat interruption\n";
                } else if (mmu.hwio_if & kInterruptionTimer) {
                    mmu.hwio_if &= ~kInterruptionTimer;
                    t += cpu.rst_50();
                    //std::cout << "catch serial interruption\n";
                } else if (mmu.hwio_if & kInterruptionSerial) {
                    mmu.hwio_if &= ~kInterruptionSerial;
                    t += cpu.rst_58();
                    //std::cout << "catch serial interruption\n";
                } else if (mmu.hwio_if & kInterruptionJoypad) {
                    mmu.hwio_if &= ~kInterruptionJoypad;
                    t += cpu.rst_60();
                    //std::cout << "catch joypad interruption\n";
                }
                gpu.step(t);
                mmu.step(t);
                clock += t;
            }

        }
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
    }
    gpu.hide();
    debugger.hide();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "usage:\n";
        std::cout << argv[0] << " <rom_file>\n";
        return 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO) >= 0 && TTF_Init() == 0) {
        emulator(argv[1]);
        TTF_Quit();
        SDL_Quit();
    } else {
        std::cerr << "Window could not be created! SDL_Error:" << SDL_GetError() << "\n";
    }
    return 0;
}

void dump_inst(uint8_t opcode, const GBCPU& cpu) {
    std::cout << std::hex;
    if (opcode != 0xcb) {
        char assembly[30];
        const char* instruction = kGBCPUInstrunctionNames[opcode];
        const int length = kInstrunctionLength[opcode];

        if (length == 2) {
            sprintf(assembly, instruction, cpu.mmu.read_byte(cpu.reg.pc));
        } else if (length == 3) {
            sprintf(assembly, instruction, cpu.mmu.read_byte(cpu.reg.pc + 1), cpu.mmu.read_byte(cpu.reg.pc));
        } else {
            strcpy(assembly, instruction);
        }

        std::cout << assembly << " : " << static_cast<uint16_t>(opcode);
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
    std::cout << "a:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.a) << ", ";
    std::cout << "f:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.f) << " ";
    std::cout << ((cpu.reg.f & 0x80) ? "z" : "-");
    std::cout << ((cpu.reg.f & 0x40) ? "n" : "-");
    std::cout << ((cpu.reg.f & 0x20) ? "h" : "-");
    std::cout << ((cpu.reg.f & 0x10) ? "c" : "-") << ", ";
    std::cout << "b:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.b) << ", ";
    std::cout << "c:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.c) << ", ";
    std::cout << "d:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.d) << ", ";
    std::cout << "e:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.e) << ", ";
    std::cout << "h:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.h) << ", ";
    std::cout << "l:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.l) << ", ";

    std::cout << "sp:" << std::setw(4) << std::setfill('0') << cpu.reg.sp << ", ";
    std::cout << "pc:" << std::setw(4) << std::setfill('0') << cpu.reg.pc;
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
    mmu.hwio_stat = 0x02;
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

void process_events(bool& running, GBJoypad& joypad) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        joypad.process_events(event);
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            running = false;
        }
    }
}
