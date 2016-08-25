#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>

#include "clock.hpp"

class Registers {
public:
    uint8_t a; // accumulator
    uint8_t f; // flag

    uint8_t b;
    uint8_t c;

    uint8_t d;
    uint8_t e;

    uint8_t h;
    uint8_t l;

    uint16_t pc;  // program counter
    uint16_t sp;  // stack pointer

    Registers() : a(0), f(0), b(0), c(0), d(0), e(0), h(0), l (0), pc(0), sp(0) {}
};

class Z80 {
private:
    int32_t acc;

public:
    Registers reg;

    Clock clock; // global system clock

    void reset();

    // Instruction Set
    void iNOP();
    void iADD_A_E();
};

#endif
