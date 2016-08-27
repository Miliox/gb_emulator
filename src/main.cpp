#include "main.hpp"

void dump_inst(uint8_t, const GBCPU&);
void dump_cpu(const GBCPU&);

int main(int argc, char** argv) {
    GBMMU mmu;
    GBCPU cpu(mmu);

    while(true) {
        // fetch
        uint8_t op = cpu.mmu.read_byte(cpu.reg.pc++);
        dump_inst(op, cpu);

        // decode
        auto& instruction = cpu.instruction_map.at(op);

        // execute
        (cpu.*instruction)();
        dump_cpu(cpu);
        std::cout << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
    std::cout << "pc: " << cpu.reg.pc << ", ";
    std::cout << std::dec;

    std::cout << "m: " << static_cast<uint16_t>(cpu.clock.m) << ", ";
    std::cout << "t: " << static_cast<uint16_t>(cpu.clock.t) << std::endl;
}
