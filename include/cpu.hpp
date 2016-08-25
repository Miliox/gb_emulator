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

    uint16_t* pc;  // program counter
    uint16_t* sp;  // stack pointer

    Registers() : a(0), f(0), b(0), c(0), d(0), e(0), h(0), l (0), pc(nullptr), sp(nullptr) {}
};

class Z80 {
private:
    int32_t acc;

public:
    Registers reg;

    Clock clock; // global system clock

    void reset();

    // Instruction Template
    void tLD_r_r(uint8_t&, const uint8_t&);
    void tADD_A_(const uint8_t&);
    void tADC_A_(const uint8_t&);
    void tADD_HL_(const uint8_t&, const uint8_t&);
    void tADC_HL_(const uint8_t&, const uint8_t&);
    void tINC_r(uint8_t&);
    void tDEC_r(uint8_t&);

    // Instruction Set
    void iNOP();

    void iLD_A_A() { tLD_r_r(reg.a, reg.a); }
    void iLD_A_B() { tLD_r_r(reg.a, reg.b); }
    void iLD_A_C() { tLD_r_r(reg.a, reg.c); }
    void iLD_A_D() { tLD_r_r(reg.a, reg.d); }
    void iLD_A_E() { tLD_r_r(reg.a, reg.e); }
    void iLD_A_H() { tLD_r_r(reg.a, reg.h); }
    void iLD_A_L() { tLD_r_r(reg.a, reg.l); }

    void iLD_B_A() { tLD_r_r(reg.b, reg.a); }
    void iLD_B_B() { tLD_r_r(reg.b, reg.b); }
    void iLD_B_C() { tLD_r_r(reg.b, reg.c); }
    void iLD_B_D() { tLD_r_r(reg.b, reg.d); }
    void iLD_B_E() { tLD_r_r(reg.b, reg.e); }
    void iLD_B_H() { tLD_r_r(reg.b, reg.h); }
    void iLD_B_L() { tLD_r_r(reg.b, reg.l); }

    void iLD_C_A() { tLD_r_r(reg.c, reg.a); }
    void iLD_C_B() { tLD_r_r(reg.c, reg.b); }
    void iLD_C_C() { tLD_r_r(reg.c, reg.c); }
    void iLD_C_D() { tLD_r_r(reg.c, reg.d); }
    void iLD_C_E() { tLD_r_r(reg.c, reg.e); }
    void iLD_C_H() { tLD_r_r(reg.c, reg.h); }
    void iLD_C_L() { tLD_r_r(reg.c, reg.l); }

    void iLD_D_A() { tLD_r_r(reg.d, reg.a); }
    void iLD_D_B() { tLD_r_r(reg.d, reg.b); }
    void iLD_D_C() { tLD_r_r(reg.d, reg.c); }
    void iLD_D_D() { tLD_r_r(reg.d, reg.d); }
    void iLD_D_E() { tLD_r_r(reg.d, reg.e); }
    void iLD_D_H() { tLD_r_r(reg.d, reg.h); }
    void iLD_D_L() { tLD_r_r(reg.d, reg.l); }

    void iLD_E_A() { tLD_r_r(reg.e, reg.a); }
    void iLD_E_B() { tLD_r_r(reg.e, reg.b); }
    void iLD_E_C() { tLD_r_r(reg.e, reg.c); }
    void iLD_E_D() { tLD_r_r(reg.e, reg.d); }
    void iLD_E_E() { tLD_r_r(reg.e, reg.e); }
    void iLD_E_H() { tLD_r_r(reg.e, reg.h); }
    void iLD_E_L() { tLD_r_r(reg.e, reg.l); }

    void iLD_H_A() { tLD_r_r(reg.h, reg.a); }
    void iLD_H_B() { tLD_r_r(reg.h, reg.b); }
    void iLD_H_C() { tLD_r_r(reg.h, reg.c); }
    void iLD_H_D() { tLD_r_r(reg.h, reg.d); }
    void iLD_H_E() { tLD_r_r(reg.h, reg.e); }
    void iLD_H_H() { tLD_r_r(reg.h, reg.h); }
    void iLD_H_L() { tLD_r_r(reg.h, reg.l); }

    void iLD_L_A() { tLD_r_r(reg.l, reg.a); }
    void iLD_L_B() { tLD_r_r(reg.l, reg.b); }
    void iLD_L_C() { tLD_r_r(reg.l, reg.c); }
    void iLD_L_D() { tLD_r_r(reg.l, reg.d); }
    void iLD_L_E() { tLD_r_r(reg.l, reg.e); }
    void iLD_L_H() { tLD_r_r(reg.l, reg.h); }
    void iLD_L_L() { tLD_r_r(reg.l, reg.l); }

    void iADD_A_A() { tADD_A_(reg.a); }
    void iADD_A_B() { tADD_A_(reg.b); }
    void iADD_A_C() { tADD_A_(reg.c); }
    void iADD_A_D() { tADD_A_(reg.d); }
    void iADD_A_E() { tADD_A_(reg.e); }
    void iADD_A_H() { tADD_A_(reg.h); }
    void iADD_A_L() { tADD_A_(reg.l); }

    void iADC_A_A() { tADC_A_(reg.a); }
    void iADC_A_B() { tADC_A_(reg.b); }
    void iADC_A_C() { tADC_A_(reg.c); }
    void iADC_A_D() { tADC_A_(reg.d); }
    void iADC_A_E() { tADC_A_(reg.e); }
    void iADC_A_H() { tADC_A_(reg.h); }
    void iADC_A_L() { tADC_A_(reg.l); }

    void iADD_HL_BC() { tADD_HL_(reg.b, reg.c); }
    void iADD_HL_DE() { tADD_HL_(reg.d, reg.e); }
    void iADD_HL_HL() { tADD_HL_(reg.h, reg.l); }
    void iADD_HL_SP();

    void iINC_A() { tINC_r(reg.a); };
    void iINC_B() { tINC_r(reg.b); };
    void iINC_C() { tINC_r(reg.c); };
    void iINC_D() { tINC_r(reg.d); };
    void iINC_E() { tINC_r(reg.e); };
    void iINC_H() { tINC_r(reg.h); };
    void iINC_L() { tINC_r(reg.l); };

    void iDEC_A() { tDEC_r(reg.a); }
    void iDEC_B() { tDEC_r(reg.b); }
    void iDEC_C() { tDEC_r(reg.c); }
    void iDEC_D() { tDEC_r(reg.d); }
    void iDEC_E() { tDEC_r(reg.e); }
    void iDEC_H() { tDEC_r(reg.h); }
    void iDEC_L() { tDEC_r(reg.l); }
};

#endif
