#include <iostream>
#include <thread>
#include <chrono>

#include "cpu.hpp"

using namespace std;

void dump_cpu(const Z80&);

int main(int argc, char** argv) {
    Z80 cpu;

    while(true) {
        // fetch
        uint8_t op = cpu.mmu.read_byte(cpu.reg.pc++);
        cout << "op:" << std::hex << static_cast<uint16_t>(op) << std::dec << endl;

        // decode
        auto& instruction = cpu.instruction_map[op];

        // execute
        (cpu.*instruction)();

        dump_cpu(cpu);
        this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

void dump_cpu(const Z80& cpu) {
    cout << "a:" << static_cast<uint16_t>(cpu.reg.a) << ", ";
    cout << "f:" << std::hex << static_cast<uint16_t>(cpu.reg.f) << std::dec << ", ";
    cout << "b:" << static_cast<uint16_t>(cpu.reg.b) << ", ";
    cout << "c:" << static_cast<uint16_t>(cpu.reg.c) << ", ";
    cout << "d:" << static_cast<uint16_t>(cpu.reg.d) << ", ";
    cout << "e:" << static_cast<uint16_t>(cpu.reg.e) << ", ";
    cout << "h:" << static_cast<uint16_t>(cpu.reg.h) << ", ";
    cout << "l:" << static_cast<uint16_t>(cpu.reg.l) << ", ";

    cout << "sp: " << cpu.reg.sp << ", ";
    cout << "pc: " << cpu.reg.pc << ", ";

    cout << "m: " << static_cast<uint16_t>(cpu.clock.m) << ", ";
    cout << "t: " << static_cast<uint16_t>(cpu.clock.t) << endl;
}
