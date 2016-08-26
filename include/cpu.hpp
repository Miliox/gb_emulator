#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>

#include "clock.hpp"
#include "mmu.hpp"

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
    MMU mmu;
    Clock clock; // global system clock
    Registers reg;

    bool interruptions_enabled;

    std::vector<void (Z80::*)()> instruction_map;
    std::vector<void (Z80::*)()> cb_instruction_map;

    Z80();
    void reset();

    // Instruction Template
    void tLD_r_r(uint8_t&, const uint8_t&);
    void tLD_r_ADDR_rr(uint8_t&, const uint8_t&, const uint8_t&);
    void tLD_ADDR_rr_r(const uint8_t&, const uint8_t&, const uint8_t&);
    void tLD_r_N(uint8_t&);
    void tLD_rr_NN(uint8_t&, uint8_t&);

    void tPUSH_rr(const uint8_t&, const uint8_t&);
    void tPOP_rr(uint8_t&, uint8_t&);

    void tADD_A_(const uint8_t&);
    void tADC_A_(const uint8_t&);
    void tADD_HL_(const uint8_t&, const uint8_t&);
    void tADC_HL_(const uint8_t&, const uint8_t&);
    void tSUB(const uint8_t&);
    void tSBC_A_(const uint8_t&);

    void tAND_r(const uint8_t&);
    void tOR_r(const uint8_t&);
    void tXOR_r(const uint8_t&);
    void tCP_r(const uint8_t&);

    void tINC_r(uint8_t&);
    void tDEC_r(uint8_t&);
    void tSWAP_r(uint8_t&);

    void tINC_rr(uint8_t&, uint8_t&);
    void tDEC_rr(uint8_t&, uint8_t&);

    void tRST(const uint16_t addr);

    // Instruction Set
    void iNotImplemented() {
        throw std::runtime_error("Instruction Not Yet Implemented");
    }

    void iNotSupported() {
        throw std::runtime_error("Instruction Not Supported");
    }

    void iNOP();
    void iHALT();
    void iRLCA();
    void iRLA();
    void iRRCA();
    void iRRA();
    void iCPL();
    void iSCF();
    void iCCF();
    void iDI();
    void iEI();

    void iLD_A_A() { tLD_r_r(reg.a, reg.a); }
    void iLD_A_B() { tLD_r_r(reg.a, reg.b); }
    void iLD_A_C() { tLD_r_r(reg.a, reg.c); }
    void iLD_A_D() { tLD_r_r(reg.a, reg.d); }
    void iLD_A_E() { tLD_r_r(reg.a, reg.e); }
    void iLD_A_H() { tLD_r_r(reg.a, reg.h); }
    void iLD_A_L() { tLD_r_r(reg.a, reg.l); }
    void iLD_A_ADDR_HL() { tLD_r_ADDR_rr(reg.a, reg.h, reg.l); }

    void iLD_B_A() { tLD_r_r(reg.b, reg.a); }
    void iLD_B_B() { tLD_r_r(reg.b, reg.b); }
    void iLD_B_C() { tLD_r_r(reg.b, reg.c); }
    void iLD_B_D() { tLD_r_r(reg.b, reg.d); }
    void iLD_B_E() { tLD_r_r(reg.b, reg.e); }
    void iLD_B_H() { tLD_r_r(reg.b, reg.h); }
    void iLD_B_L() { tLD_r_r(reg.b, reg.l); }
    void iLD_B_ADDR_HL() { tLD_r_ADDR_rr(reg.b, reg.h, reg.l); }

    void iLD_C_A() { tLD_r_r(reg.c, reg.a); }
    void iLD_C_B() { tLD_r_r(reg.c, reg.b); }
    void iLD_C_C() { tLD_r_r(reg.c, reg.c); }
    void iLD_C_D() { tLD_r_r(reg.c, reg.d); }
    void iLD_C_E() { tLD_r_r(reg.c, reg.e); }
    void iLD_C_H() { tLD_r_r(reg.c, reg.h); }
    void iLD_C_L() { tLD_r_r(reg.c, reg.l); }
    void iLD_C_ADDR_HL() { tLD_r_ADDR_rr(reg.c, reg.h, reg.l); }

    void iLD_D_A() { tLD_r_r(reg.d, reg.a); }
    void iLD_D_B() { tLD_r_r(reg.d, reg.b); }
    void iLD_D_C() { tLD_r_r(reg.d, reg.c); }
    void iLD_D_D() { tLD_r_r(reg.d, reg.d); }
    void iLD_D_E() { tLD_r_r(reg.d, reg.e); }
    void iLD_D_H() { tLD_r_r(reg.d, reg.h); }
    void iLD_D_L() { tLD_r_r(reg.d, reg.l); }
    void iLD_D_ADDR_HL() { tLD_r_ADDR_rr(reg.d, reg.h, reg.l); }

    void iLD_E_A() { tLD_r_r(reg.e, reg.a); }
    void iLD_E_B() { tLD_r_r(reg.e, reg.b); }
    void iLD_E_C() { tLD_r_r(reg.e, reg.c); }
    void iLD_E_D() { tLD_r_r(reg.e, reg.d); }
    void iLD_E_E() { tLD_r_r(reg.e, reg.e); }
    void iLD_E_H() { tLD_r_r(reg.e, reg.h); }
    void iLD_E_L() { tLD_r_r(reg.e, reg.l); }
    void iLD_E_ADDR_HL() { tLD_r_ADDR_rr(reg.e, reg.h, reg.l); }

    void iLD_H_A() { tLD_r_r(reg.h, reg.a); }
    void iLD_H_B() { tLD_r_r(reg.h, reg.b); }
    void iLD_H_C() { tLD_r_r(reg.h, reg.c); }
    void iLD_H_D() { tLD_r_r(reg.h, reg.d); }
    void iLD_H_E() { tLD_r_r(reg.h, reg.e); }
    void iLD_H_H() { tLD_r_r(reg.h, reg.h); }
    void iLD_H_L() { tLD_r_r(reg.h, reg.l); }
    void iLD_H_ADDR_HL() { tLD_r_ADDR_rr(reg.h, reg.h, reg.l); }

    void iLD_L_A() { tLD_r_r(reg.l, reg.a); }
    void iLD_L_B() { tLD_r_r(reg.l, reg.b); }
    void iLD_L_C() { tLD_r_r(reg.l, reg.c); }
    void iLD_L_D() { tLD_r_r(reg.l, reg.d); }
    void iLD_L_E() { tLD_r_r(reg.l, reg.e); }
    void iLD_L_H() { tLD_r_r(reg.l, reg.h); }
    void iLD_L_L() { tLD_r_r(reg.l, reg.l); }
    void iLD_L_ADDR_HL() { tLD_r_ADDR_rr(reg.l, reg.h, reg.l); }

    void iLD_SP_HL();

    void iLD_ADDR_HL_A() { tLD_ADDR_rr_r(reg.h, reg.l, reg.a); }
    void iLD_ADDR_HL_B() { tLD_ADDR_rr_r(reg.h, reg.l, reg.b); }
    void iLD_ADDR_HL_C() { tLD_ADDR_rr_r(reg.h, reg.l, reg.c); }
    void iLD_ADDR_HL_D() { tLD_ADDR_rr_r(reg.h, reg.l, reg.d); }
    void iLD_ADDR_HL_E() { tLD_ADDR_rr_r(reg.h, reg.l, reg.e); }
    void iLD_ADDR_HL_H() { tLD_ADDR_rr_r(reg.h, reg.l, reg.h); }
    void iLD_ADDR_HL_L() { tLD_ADDR_rr_r(reg.h, reg.l, reg.l); }

    void iLD_ADDR_BC_A() { tLD_ADDR_rr_r(reg.b, reg.c, reg.a); }
    void iLD_ADDR_DE_A() { tLD_ADDR_rr_r(reg.d, reg.e, reg.a); }

    void iLD_A_ADDR_BC() { tLD_r_ADDR_rr(reg.a, reg.b, reg.c); }
    void iLD_A_ADDR_DE() { tLD_r_ADDR_rr(reg.a, reg.d, reg.e); }
    void iLD_A_ADDR_NN();
    void iLD_ADDR_NN_A();

    void iLD_A_N() { tLD_r_N(reg.a); }
    void iLD_B_N() { tLD_r_N(reg.b); }
    void iLD_C_N() { tLD_r_N(reg.c); }
    void iLD_D_N() { tLD_r_N(reg.d); }
    void iLD_E_N() { tLD_r_N(reg.e); }
    void iLD_H_N() { tLD_r_N(reg.h); }
    void iLD_L_N() { tLD_r_N(reg.l); }

    void iLD_BC_NN() { tLD_rr_NN(reg.b, reg.c); }
    void iLD_DE_NN() { tLD_rr_NN(reg.d, reg.e); }
    void iLD_HL_NN() { tLD_rr_NN(reg.h, reg.l); }
    void iLD_SP_NN();

    void iLD_A_OFFSET_ADDR_C();
    void iLD_OFFSET_ADDR_C_A();

    void iLDH_OFFSET_N_A();
    void iLDH_A_OFFSET_N();

    void iLDI_A_ADDR_HL();
    void iLDI_ADDR_HL_A();

    void iLDD_A_ADDR_HL();
    void iLDD_ADDR_HL_A();

    void iADD_A_A() { tADD_A_(reg.a); }
    void iADD_A_B() { tADD_A_(reg.b); }
    void iADD_A_C() { tADD_A_(reg.c); }
    void iADD_A_D() { tADD_A_(reg.d); }
    void iADD_A_E() { tADD_A_(reg.e); }
    void iADD_A_H() { tADD_A_(reg.h); }
    void iADD_A_L() { tADD_A_(reg.l); }
    void iADD_A_ADDR_HL();
    void iADD_A_N();

    void iADC_A_A() { tADC_A_(reg.a); }
    void iADC_A_B() { tADC_A_(reg.b); }
    void iADC_A_C() { tADC_A_(reg.c); }
    void iADC_A_D() { tADC_A_(reg.d); }
    void iADC_A_E() { tADC_A_(reg.e); }
    void iADC_A_H() { tADC_A_(reg.h); }
    void iADC_A_L() { tADC_A_(reg.l); }
    void iADC_A_ADDR_HL();
    void iADC_A_N();

    void iADD_HL_BC() { tADD_HL_(reg.b, reg.c); }
    void iADD_HL_DE() { tADD_HL_(reg.d, reg.e); }
    void iADD_HL_HL() { tADD_HL_(reg.h, reg.l); }
    void iADD_HL_SP();

    void iSUB_A() { tSUB(reg.a); }
    void iSUB_B() { tSUB(reg.b); }
    void iSUB_C() { tSUB(reg.c); }
    void iSUB_D() { tSUB(reg.d); }
    void iSUB_E() { tSUB(reg.e); }
    void iSUB_H() { tSUB(reg.h); }
    void iSUB_L() { tSUB(reg.l); }
    void iSUB_ADDR_HL();
    void iSUB_N();

    void iSBC_A_A() { tSBC_A_(reg.a); }
    void iSBC_A_B() { tSBC_A_(reg.b); }
    void iSBC_A_C() { tSBC_A_(reg.c); }
    void iSBC_A_D() { tSBC_A_(reg.d); }
    void iSBC_A_E() { tSBC_A_(reg.e); }
    void iSBC_A_H() { tSBC_A_(reg.h); }
    void iSBC_A_L() { tSBC_A_(reg.l); }
    void iSBC_A_ADDR_HL();
    void iSBC_A_N();

    void iAND_A() { tAND_r(reg.a); }
    void iAND_B() { tAND_r(reg.b); }
    void iAND_C() { tAND_r(reg.c); }
    void iAND_D() { tAND_r(reg.d); }
    void iAND_E() { tAND_r(reg.e); }
    void iAND_H() { tAND_r(reg.h); }
    void iAND_L() { tAND_r(reg.l); }
    void iAND_ADDR_HL();
    void iAND_N();

    void iOR_A() { tOR_r(reg.a); }
    void iOR_B() { tOR_r(reg.b); }
    void iOR_C() { tOR_r(reg.c); }
    void iOR_D() { tOR_r(reg.d); }
    void iOR_E() { tOR_r(reg.e); }
    void iOR_H() { tOR_r(reg.h); }
    void iOR_L() { tOR_r(reg.l); }
    void iOR_ADDR_HL();
    void iOR_N();

    void iXOR_A() { tXOR_r(reg.a); }
    void iXOR_B() { tXOR_r(reg.b); }
    void iXOR_C() { tXOR_r(reg.c); }
    void iXOR_D() { tXOR_r(reg.d); }
    void iXOR_E() { tXOR_r(reg.e); }
    void iXOR_H() { tXOR_r(reg.h); }
    void iXOR_L() { tXOR_r(reg.l); }
    void iXOR_ADDR_HL();
    void iXOR_N();

    void iCP_A() { tCP_r(reg.a); }
    void iCP_B() { tCP_r(reg.b); }
    void iCP_C() { tCP_r(reg.c); }
    void iCP_D() { tCP_r(reg.d); }
    void iCP_E() { tCP_r(reg.e); }
    void iCP_H() { tCP_r(reg.h); }
    void iCP_L() { tCP_r(reg.l); }
    void iCP_ADDR_HL();
    void iCP_N();

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

    void iSWAP_A() { tSWAP_r(reg.a); }
    void iSWAP_B() { tSWAP_r(reg.b); }
    void iSWAP_C() { tSWAP_r(reg.c); }
    void iSWAP_D() { tSWAP_r(reg.d); }
    void iSWAP_E() { tSWAP_r(reg.e); }
    void iSWAP_H() { tSWAP_r(reg.h); }
    void iSWAP_L() { tSWAP_r(reg.l); }

    void iINC_BC() { tINC_rr(reg.b, reg.c); }
    void iINC_DE() { tINC_rr(reg.d, reg.e); }
    void iINC_HL() { tINC_rr(reg.h, reg.l); }
    void iINC_SP() { reg.sp += 1; clock += Clock(2); }
    void iINC_ADDR_HL();

    void iDEC_BC() { tDEC_rr(reg.b, reg.c); }
    void iDEC_DE() { tDEC_rr(reg.d, reg.e); }
    void iDEC_HL() { tDEC_rr(reg.h, reg.l); }
    void iDEC_SP() { reg.sp -= 1; clock += Clock(2); }
    void iDEC_ADDR_HL();

    void iPUSH_AF() { tPUSH_rr(reg.a, reg.f); }
    void iPUSH_BC() { tPUSH_rr(reg.b, reg.c); }
    void iPUSH_DE() { tPUSH_rr(reg.d, reg.e); }
    void iPUSH_HL() { tPUSH_rr(reg.h, reg.l); }

    void iPOP_AF() { tPOP_rr(reg.a, reg.f); }
    void iPOP_BC() { tPOP_rr(reg.b, reg.c); }
    void iPOP_DE() { tPOP_rr(reg.d, reg.e); }
    void iPOP_HL() { tPOP_rr(reg.h, reg.l); }

    void iRST_00H() { tRST(0x00); };
    void iRST_08H() { tRST(0x08); };
    void iRST_10H() { tRST(0x10); };
    void iRST_18H() { tRST(0x18); };
    void iRST_20H() { tRST(0x20); };
    void iRST_28H() { tRST(0x28); };
    void iRST_30H() { tRST(0x30); };
    void iRST_38H() { tRST(0x38); };

    void iCALL();
    void iCALL_Z();
    void iCALL_NZ();
    void iCALL_C();
    void iCALL_NC();

    void iRET();
    void iRETI();
    void iRET_Z();
    void iRET_NZ();
    void iRET_C();
    void iRET_NC();

    void iJP();
    void iJP_Z();
    void iJP_NZ();
    void iJP_C();
    void iJP_NC();
    void iJP_HL();

    void iJR();
    void iJR_Z();
    void iJR_NZ();
    void iJR_C();
    void iJR_NC();
};

#endif
