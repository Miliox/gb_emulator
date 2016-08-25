#include <iostream>

#include "cpu.hpp"

using namespace std;

void print_cpu(Z80& cpu) {
    cout << "Z80.reg.a: " << (uint16_t) cpu.reg.a << endl;
    cout << "Z80.reg.b: " << (uint16_t) cpu.reg.b << endl;
    cout << "Z80.reg.c: " << (uint16_t) cpu.reg.c << endl;
    cout << "Z80.reg.d: " << (uint16_t) cpu.reg.d << endl;
    cout << "Z80.reg.e: " << (uint16_t) cpu.reg.e << endl;
    cout << "Z80.reg.f: " << (uint16_t) cpu.reg.f << endl;
    cout << "Z80.reg.h: " << (uint16_t) cpu.reg.h << endl;
    cout << "Z80.reg.l: " << (uint16_t) cpu.reg.l << endl;
    cout << "Z80.reg.sp: " << (uint16_t) cpu.reg.sp << endl;
    cout << "Z80.reg.pc: " << (uint16_t) cpu.reg.pc << endl;
    cout << "Z80.clock.m: " << (uint16_t) cpu.clock.m << endl;
    cout << "Z80.clock.t: " << (uint16_t) cpu.clock.t << endl;
}

int main(int argc, char** argv) {
    Z80 cpu;
    cpu.reset();

    cout << "reset" << endl;
    print_cpu(cpu);
    cout << endl;

    cpu.iNOP();

    cout << "nop" << endl;
    print_cpu(cpu);
    cout << endl;

    cpu.reg.a = 5;
    cpu.reg.e = 10;

    cout << "add A(5) and E(10)" << endl;
    cpu.iADD_A_E();
    print_cpu(cpu);

    return 0;
}
