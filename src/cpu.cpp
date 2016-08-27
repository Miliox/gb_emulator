#include "cpu.hpp"

#include <cstring>

const uint16_t kFlagZ = 1 << 7;
const uint16_t kFlagN = 1 << 6;
const uint16_t kFlagH = 1 << 5;
const uint16_t kFlagC = 1 << 4;

inline uint16_t check_z(int32_t value) {
    return (value == 0) ? kFlagZ : 0;
}

inline uint16_t check_h(uint32_t value) {
    return (value & 0x10) == 0x10 ? kFlagH : 0;
}

inline uint16_t check_c(int32_t value) {
    return (value > 0xff || value < 0) ? kFlagC : 0;
}

inline uint16_t check_h2(uint32_t value) {
    return (value & 0x10) == 0x1000 ? kFlagH : 0;
}

inline uint16_t check_c2(int32_t value) {
    return (value > 0xffff || value < 0) ? kFlagC : 0;
}

inline uint16_t combine16(const uint8_t& msb, const uint8_t& lsb) {
    return static_cast<uint16_t>((msb << 8) + lsb);
}

inline void split16(const uint16_t& src, uint8_t& msb, uint8_t& lsb) {
    msb = static_cast<uint8_t>(src >> 8);
    lsb = static_cast<uint8_t>(src);
}

Z80::Z80() {
    instruction_map.reserve(256);
    cb_instruction_map =
        std::vector<void (Z80::*)()>(256, &Z80::iNotImplemented);

    instruction_map[0x00] = &Z80::iNOP;
    instruction_map[0x01] = &Z80::iLD_BC_NN;
    instruction_map[0x02] = &Z80::iLD_ADDR_BC_A;
    instruction_map[0x03] = &Z80::iINC_BC;
    instruction_map[0x04] = &Z80::iINC_B;
    instruction_map[0x05] = &Z80::iDEC_B;
    instruction_map[0x06] = &Z80::iLD_B_N;
    instruction_map[0x07] = &Z80::iRLCA;
    instruction_map[0x08] = &Z80::iLD_ADDR_NN_SP;
    instruction_map[0x09] = &Z80::iADD_HL_BC;
    instruction_map[0x0A] = &Z80::iLD_A_ADDR_BC;
    instruction_map[0x0B] = &Z80::iDEC_BC;
    instruction_map[0x0C] = &Z80::iINC_C;
    instruction_map[0x0D] = &Z80::iDEC_C;
    instruction_map[0x0E] = &Z80::iLD_C_N;
    instruction_map[0x0F] = &Z80::iRRCA;

    instruction_map[0x10] = &Z80::iSTOP;
    instruction_map[0x11] = &Z80::iLD_DE_NN;
    instruction_map[0x12] = &Z80::iLD_ADDR_DE_A;
    instruction_map[0x13] = &Z80::iINC_DE;
    instruction_map[0x14] = &Z80::iINC_D;
    instruction_map[0x15] = &Z80::iDEC_D;
    instruction_map[0x16] = &Z80::iLD_D_N;
    instruction_map[0x17] = &Z80::iRLA;
    instruction_map[0x18] = &Z80::iJR;
    instruction_map[0x19] = &Z80::iADD_HL_DE;
    instruction_map[0x1A] = &Z80::iLD_A_ADDR_DE;
    instruction_map[0x1B] = &Z80::iDEC_DE;
    instruction_map[0x1C] = &Z80::iINC_E;
    instruction_map[0x1D] = &Z80::iDEC_E;
    instruction_map[0x1E] = &Z80::iLD_E_N;
    instruction_map[0x1F] = &Z80::iRRA;

    instruction_map[0x20] = &Z80::iJR_NZ;
    instruction_map[0x21] = &Z80::iLD_HL_NN;
    instruction_map[0x22] = &Z80::iLDI_ADDR_HL_A;
    instruction_map[0x23] = &Z80::iINC_HL;
    instruction_map[0x24] = &Z80::iINC_H;
    instruction_map[0x25] = &Z80::iDEC_H;
    instruction_map[0x26] = &Z80::iLD_H_N;
    instruction_map[0x27] = &Z80::iNotImplemented; // DAA
    instruction_map[0x28] = &Z80::iJR_Z;
    instruction_map[0x29] = &Z80::iADD_HL_HL;
    instruction_map[0x2A] = &Z80::iLDI_A_ADDR_HL;
    instruction_map[0x2B] = &Z80::iDEC_HL;
    instruction_map[0x2C] = &Z80::iINC_L;
    instruction_map[0x2D] = &Z80::iDEC_L;
    instruction_map[0x2E] = &Z80::iLD_L_N;
    instruction_map[0x2F] = &Z80::iCPL;

    instruction_map[0x30] = &Z80::iJR_NC;
    instruction_map[0x31] = &Z80::iLD_SP_NN;
    instruction_map[0x32] = &Z80::iLDD_ADDR_HL_A;
    instruction_map[0x33] = &Z80::iINC_SP;
    instruction_map[0x34] = &Z80::iINC_ADDR_HL;
    instruction_map[0x35] = &Z80::iDEC_ADDR_HL;
    instruction_map[0x36] = &Z80::iLD_ADDR_HL_N;
    instruction_map[0x37] = &Z80::iSCF;
    instruction_map[0x38] = &Z80::iJR_C;
    instruction_map[0x39] = &Z80::iADD_HL_SP;
    instruction_map[0x3A] = &Z80::iLDD_A_ADDR_HL;
    instruction_map[0x3B] = &Z80::iDEC_SP;
    instruction_map[0x3C] = &Z80::iINC_A;
    instruction_map[0x3D] = &Z80::iDEC_A;
    instruction_map[0x3E] = &Z80::iLD_A_N;
    instruction_map[0x3F] = &Z80::iCCF;

    instruction_map[0x40] = &Z80::iLD_B_B;
    instruction_map[0x41] = &Z80::iLD_B_C;
    instruction_map[0x42] = &Z80::iLD_B_D;
    instruction_map[0x43] = &Z80::iLD_B_E;
    instruction_map[0x44] = &Z80::iLD_B_H;
    instruction_map[0x45] = &Z80::iLD_B_L;
    instruction_map[0x46] = &Z80::iLD_B_ADDR_HL;
    instruction_map[0x47] = &Z80::iLD_B_A;
    instruction_map[0x48] = &Z80::iLD_C_B;
    instruction_map[0x49] = &Z80::iLD_C_C;
    instruction_map[0x4A] = &Z80::iLD_C_D;
    instruction_map[0x4B] = &Z80::iLD_C_E;
    instruction_map[0x4C] = &Z80::iLD_C_H;
    instruction_map[0x4D] = &Z80::iLD_C_L;
    instruction_map[0x4E] = &Z80::iLD_C_ADDR_HL;
    instruction_map[0x4F] = &Z80::iLD_C_A;

    instruction_map[0x50] = &Z80::iLD_D_B;
    instruction_map[0x51] = &Z80::iLD_D_C;
    instruction_map[0x52] = &Z80::iLD_D_D;
    instruction_map[0x53] = &Z80::iLD_D_E;
    instruction_map[0x54] = &Z80::iLD_D_H;
    instruction_map[0x55] = &Z80::iLD_D_L;
    instruction_map[0x56] = &Z80::iLD_D_ADDR_HL;
    instruction_map[0x57] = &Z80::iLD_D_A;
    instruction_map[0x58] = &Z80::iLD_E_B;
    instruction_map[0x59] = &Z80::iLD_E_C;
    instruction_map[0x5A] = &Z80::iLD_E_D;
    instruction_map[0x5B] = &Z80::iLD_E_E;
    instruction_map[0x5C] = &Z80::iLD_E_H;
    instruction_map[0x5D] = &Z80::iLD_E_L;
    instruction_map[0x5E] = &Z80::iLD_E_ADDR_HL;
    instruction_map[0x5F] = &Z80::iLD_E_A;

    instruction_map[0x60] = &Z80::iLD_H_B;
    instruction_map[0x61] = &Z80::iLD_H_C;
    instruction_map[0x62] = &Z80::iLD_H_D;
    instruction_map[0x63] = &Z80::iLD_H_E;
    instruction_map[0x64] = &Z80::iLD_H_H;
    instruction_map[0x65] = &Z80::iLD_H_L;
    instruction_map[0x66] = &Z80::iLD_H_ADDR_HL;
    instruction_map[0x67] = &Z80::iLD_H_A;
    instruction_map[0x68] = &Z80::iLD_L_B;
    instruction_map[0x69] = &Z80::iLD_L_C;
    instruction_map[0x6A] = &Z80::iLD_L_D;
    instruction_map[0x6B] = &Z80::iLD_L_E;
    instruction_map[0x6C] = &Z80::iLD_L_H;
    instruction_map[0x6D] = &Z80::iLD_L_L;
    instruction_map[0x6E] = &Z80::iLD_L_ADDR_HL;
    instruction_map[0x6F] = &Z80::iLD_L_A;

    instruction_map[0x70] = &Z80::iLD_ADDR_HL_B;
    instruction_map[0x71] = &Z80::iLD_ADDR_HL_C;
    instruction_map[0x72] = &Z80::iLD_ADDR_HL_D;
    instruction_map[0x73] = &Z80::iLD_ADDR_HL_E;
    instruction_map[0x74] = &Z80::iLD_ADDR_HL_H;
    instruction_map[0x75] = &Z80::iLD_ADDR_HL_L;
    instruction_map[0x76] = &Z80::iHALT;
    instruction_map[0x77] = &Z80::iLD_ADDR_HL_A;
    instruction_map[0x78] = &Z80::iLD_A_B;
    instruction_map[0x79] = &Z80::iLD_A_C;
    instruction_map[0x7A] = &Z80::iLD_A_D;
    instruction_map[0x7B] = &Z80::iLD_A_E;
    instruction_map[0x7C] = &Z80::iLD_A_H;
    instruction_map[0x7D] = &Z80::iLD_A_L;
    instruction_map[0x7E] = &Z80::iLD_A_ADDR_HL;
    instruction_map[0x7F] = &Z80::iLD_A_A;

    instruction_map[0x80] = &Z80::iADD_A_B;
    instruction_map[0x81] = &Z80::iADD_A_C;
    instruction_map[0x82] = &Z80::iADD_A_D;
    instruction_map[0x83] = &Z80::iADD_A_E;
    instruction_map[0x84] = &Z80::iADD_A_H;
    instruction_map[0x85] = &Z80::iADD_A_L;
    instruction_map[0x86] = &Z80::iADD_A_ADDR_HL;
    instruction_map[0x87] = &Z80::iADD_A_A;
    instruction_map[0x88] = &Z80::iADC_A_B;
    instruction_map[0x89] = &Z80::iADC_A_C;
    instruction_map[0x8A] = &Z80::iADC_A_D;
    instruction_map[0x8B] = &Z80::iADC_A_E;
    instruction_map[0x8C] = &Z80::iADC_A_H;
    instruction_map[0x8D] = &Z80::iADC_A_L;
    instruction_map[0x8E] = &Z80::iADC_A_ADDR_HL;
    instruction_map[0x8F] = &Z80::iADC_A_A;

    instruction_map[0x90] = &Z80::iSUB_B;
    instruction_map[0x91] = &Z80::iSUB_C;
    instruction_map[0x92] = &Z80::iSUB_D;
    instruction_map[0x93] = &Z80::iSUB_E;
    instruction_map[0x94] = &Z80::iSUB_H;
    instruction_map[0x95] = &Z80::iSUB_L;
    instruction_map[0x96] = &Z80::iSUB_ADDR_HL;
    instruction_map[0x97] = &Z80::iSUB_A;
    instruction_map[0x98] = &Z80::iSBC_A_B;
    instruction_map[0x99] = &Z80::iSBC_A_C;
    instruction_map[0x9A] = &Z80::iSBC_A_D;
    instruction_map[0x9B] = &Z80::iSBC_A_E;
    instruction_map[0x9C] = &Z80::iSBC_A_H;
    instruction_map[0x9D] = &Z80::iSBC_A_L;
    instruction_map[0x9E] = &Z80::iSBC_A_ADDR_HL;
    instruction_map[0x9F] = &Z80::iSBC_A_A;

    instruction_map[0xA0] = &Z80::iAND_B;
    instruction_map[0xA1] = &Z80::iAND_C;
    instruction_map[0xA2] = &Z80::iAND_D;
    instruction_map[0xA3] = &Z80::iAND_E;
    instruction_map[0xA4] = &Z80::iAND_H;
    instruction_map[0xA5] = &Z80::iAND_L;
    instruction_map[0xA6] = &Z80::iAND_ADDR_HL;
    instruction_map[0xA7] = &Z80::iAND_A;
    instruction_map[0xA8] = &Z80::iXOR_B;
    instruction_map[0xA9] = &Z80::iXOR_C;
    instruction_map[0xAA] = &Z80::iXOR_D;
    instruction_map[0xAB] = &Z80::iXOR_E;
    instruction_map[0xAC] = &Z80::iXOR_H;
    instruction_map[0xAD] = &Z80::iXOR_L;
    instruction_map[0xAE] = &Z80::iXOR_ADDR_HL;
    instruction_map[0xAF] = &Z80::iXOR_A;

    instruction_map[0xB0] = &Z80::iOR_B;
    instruction_map[0xB1] = &Z80::iOR_C;
    instruction_map[0xB2] = &Z80::iOR_D;
    instruction_map[0xB3] = &Z80::iOR_E;
    instruction_map[0xB4] = &Z80::iOR_H;
    instruction_map[0xB5] = &Z80::iOR_L;
    instruction_map[0xB6] = &Z80::iOR_ADDR_HL;
    instruction_map[0xB7] = &Z80::iOR_A;
    instruction_map[0xB8] = &Z80::iCP_B;
    instruction_map[0xB9] = &Z80::iCP_C;
    instruction_map[0xBA] = &Z80::iCP_D;
    instruction_map[0xBB] = &Z80::iCP_E;
    instruction_map[0xBC] = &Z80::iCP_H;
    instruction_map[0xBD] = &Z80::iCP_L;
    instruction_map[0xBE] = &Z80::iCP_ADDR_HL;
    instruction_map[0xBF] = &Z80::iCP_A;

    instruction_map[0xC0] = &Z80::iRET_NZ;
    instruction_map[0xC1] = &Z80::iPOP_BC;
    instruction_map[0xC2] = &Z80::iJP_NZ;
    instruction_map[0xC3] = &Z80::iJP;
    instruction_map[0xC4] = &Z80::iCALL_NZ;
    instruction_map[0xC5] = &Z80::iPUSH_BC;
    instruction_map[0xC6] = &Z80::iADD_A_N;
    instruction_map[0xC7] = &Z80::iRST_00H;
    instruction_map[0xC8] = &Z80::iRET_Z;
    instruction_map[0xC9] = &Z80::iRET;
    instruction_map[0xCA] = &Z80::iJP_Z;
    instruction_map[0xCB] = &Z80::iCB_CallBranch;
    instruction_map[0xCC] = &Z80::iCALL_Z;
    instruction_map[0xCD] = &Z80::iCALL;
    instruction_map[0xCE] = &Z80::iADC_A_N;
    instruction_map[0xCF] = &Z80::iRST_08H;

    instruction_map[0xD0] = &Z80::iRET_NC;
    instruction_map[0xD1] = &Z80::iPOP_DE;
    instruction_map[0xD2] = &Z80::iJP_NC;
    instruction_map[0xD3] = &Z80::iNotSupported;
    instruction_map[0xD4] = &Z80::iCALL_NC;
    instruction_map[0xD5] = &Z80::iPUSH_DE;
    instruction_map[0xD6] = &Z80::iSUB_N;
    instruction_map[0xD7] = &Z80::iRST_10H;
    instruction_map[0xD8] = &Z80::iRET_C;
    instruction_map[0xD9] = &Z80::iRETI;
    instruction_map[0xDA] = &Z80::iJP_C;
    instruction_map[0xDB] = &Z80::iNotSupported;
    instruction_map[0xDC] = &Z80::iCALL_C;
    instruction_map[0xDD] = &Z80::iNotSupported;
    instruction_map[0xDE] = &Z80::iSBC_A_N;
    instruction_map[0xDF] = &Z80::iRST_18H;

    instruction_map[0xE0] = &Z80::iLDH_OFFSET_N_A;
    instruction_map[0xE1] = &Z80::iPOP_HL;
    instruction_map[0xE2] = &Z80::iLD_OFFSET_ADDR_C_A;
    instruction_map[0xE3] = &Z80::iNotSupported;
    instruction_map[0xE4] = &Z80::iNotSupported;
    instruction_map[0xE5] = &Z80::iPUSH_HL;
    instruction_map[0xE6] = &Z80::iAND_N;
    instruction_map[0xE7] = &Z80::iRST_20H;
    instruction_map[0xE8] = &Z80::iADD_SP_N;
    instruction_map[0xE9] = &Z80::iJP_HL;
    instruction_map[0xEA] = &Z80::iLD_ADDR_NN_A;
    instruction_map[0xEB] = &Z80::iNotSupported;
    instruction_map[0xEC] = &Z80::iNotSupported;
    instruction_map[0xED] = &Z80::iNotSupported;
    instruction_map[0xEE] = &Z80::iXOR_N;
    instruction_map[0xEF] = &Z80::iRST_28H;

    instruction_map[0xF0] = &Z80::iLDH_A_OFFSET_N;
    instruction_map[0xF1] = &Z80::iPOP_AF;
    instruction_map[0xF2] = &Z80::iLD_A_OFFSET_ADDR_C;
    instruction_map[0xF3] = &Z80::iDI;
    instruction_map[0xF4] = &Z80::iNotSupported;
    instruction_map[0xF5] = &Z80::iPUSH_AF;
    instruction_map[0xF6] = &Z80::iOR_N;
    instruction_map[0xF7] = &Z80::iRST_30H;
    instruction_map[0xF8] = &Z80::iLD_HL_SPN;
    instruction_map[0xF9] = &Z80::iLD_SP_HL;
    instruction_map[0xFA] = &Z80::iLD_A_ADDR_NN;
    instruction_map[0xFB] = &Z80::iEI;
    instruction_map[0xFC] = &Z80::iNotSupported;
    instruction_map[0xFD] = &Z80::iNotSupported;
    instruction_map[0xFE] = &Z80::iCP_N;
    instruction_map[0xFF] = &Z80::iRST_38H;

}

void Z80::reset() {
    std::memset(this, 0, sizeof(Z80));
}

// Template Instruction

/**
 * LD r1,r2
 *
 * Put value r2 into r1
 */
void Z80::tLD_r_r(uint8_t& dst, const uint8_t& src) {
    dst = src;
    clock += Clock(1);
}

/**
 * LD r,(HL)
 *
 * Put value from address HL into r
 */
void Z80::tLD_r_ADDR_rr(uint8_t& dst, const uint8_t& rh, const uint8_t& rl) {
    uint16_t addr = combine16(rh, rl);
    dst = mmu.read_byte(addr);

    clock += Clock(2);
}

/**
 * LD (HL),r
 *
 * Put value r into address from HL
 */
void Z80::tLD_ADDR_rr_r(const uint8_t& rh, const uint8_t& rl, const uint8_t& src) {
    uint16_t addr = combine16(rh, rl);
    mmu.write_byte(addr, src);

    clock += Clock(2);
}

void Z80::tLD_r_N(uint8_t& r) {
    r = mmu.read_byte(reg.pc++);
    clock += Clock(2);
}

void Z80::tLD_rr_NN(uint8_t& rh, uint8_t& rl) {
    rl = mmu.read_byte(reg.pc++);
    rh = mmu.read_byte(reg.pc++);
    clock += Clock(3);
}

/**
 * Push register pair onto the stack
 * Decrement stack pointer twice
*/
void Z80::tPUSH_rr(const uint8_t& rh, const uint8_t& rl) {
    mmu.write_byte(reg.sp--, rl);
    mmu.write_byte(reg.sp--, rh);
    clock += Clock(4);
}

/**
 * Pop two bytes from the stack into register pair
 * Increment stack pointer twice
*/
void Z80::tPOP_rr(uint8_t& rh, uint8_t& rl) {
    rh = mmu.read_byte(reg.sp++);
    rl = mmu.read_byte(reg.sp++);
    clock += Clock(3);
}

/**
 * ADD A,r
 *
 * Add r to A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Set if carry from bit 3
 * C - Set if carry from bit 7
 */
void Z80::tADD_A_(const uint8_t& r) {
    acc = reg.a + r;
    reg.f = check_z(acc & 0xff) + check_h(acc) + check_c(acc);
    reg.a = static_cast<uint8_t>(acc);

    clock += Clock(1);
}

/**
 * ADD HL,rr
 *
 * ADD rr to HL
 * rr: BC, DE, HL, SP
 *
 * Flags Affected:
 * Z - Not affected
 * N - Reset
 * H - Set if carry from bit 11
 * C - Set if carry from bit 15
 */
void Z80::tADD_HL_(const uint8_t& rh, const uint8_t& rl) {
    acc = (reg.h << 8) + reg.l;
    acc += (rh << 8) + rl;

    reg.f = (reg.f & kFlagZ) + check_h2(acc) + check_c2(acc);
    split16(acc, reg.h, reg.l);

    clock += Clock(2);
}

/**
 * ADC A,r
 *
 * ADC (r + carry) to A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Set if carry from bit 3
 * C - Set if carry from bit 7
 */
void Z80::tADC_A_(const uint8_t& r) {
    acc = reg.a + r;
    acc += reg.f & kFlagC ? 1 : 0;

    reg.f = check_z(acc & 0xff) + check_h(acc) + check_c(acc);
    reg.a = static_cast<uint8_t>(acc);

    clock += Clock(1);
}

/**
 * SUB r
 *
 * Subtract r from A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Set
 * H - Set if borrow from bit 4
 * C - Set if no borrow
 */
void Z80::tSUB(const uint8_t& r) {
    acc = reg.a - r;

    reg.f = check_z(acc & 0xff) | kFlagN;
    reg.f |= (reg.a < r) ? kFlagC : 0;
    reg.f |= ((reg.a > 0xff && r > 0xff) || reg.a < r) ? kFlagH : 0;
    reg.a = static_cast<uint8_t>(acc);

    clock += Clock(1);
}

/**
 * SBC r
 *
 * Subtract r from A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Set
 * H - Set if borrow from bit 4
 * C - Set if no borrow
 */
void Z80::tSBC_A_(const uint8_t& r) {
    acc = reg.a - r;
    acc -= reg.f & kFlagC ? 1 : 0;

    reg.f = check_z(acc & 0xff) | kFlagN;
    reg.f |= (reg.a < r) ? kFlagC : 0;
    reg.f |= ((reg.a > 0xff && r > 0xff) || reg.a < r) ? kFlagH : 0;
    reg.a = static_cast<uint8_t>(acc);

    clock += Clock(1);
}

/**
 * AND r
 *
 * Logical AND r with A, result in A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Set
 * C - Reset
 */
void Z80::tAND_r(const uint8_t& r) {
    reg.a &= r;

    reg.f = check_z(reg.a) | kFlagH;

    clock += Clock(1);
}

/**
 * OR r
 *
 * Logical OR r with A, result in A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Reset
 */
void Z80::tOR_r(const uint8_t& r) {
    reg.a |= r;

    reg.f = check_z(reg.a);

    clock += Clock(1);
}

/**
 * XOR r
 *
 * Logical exclusive OR r with register A, result in A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Reset
 */
void Z80::tXOR_r(const uint8_t& r) {
    reg.a = static_cast<uint8_t>((!r & reg.a) | (r & !reg.a));
    reg.f = check_z(reg.a);

    clock += Clock(1);
}

/**
 * CP r
 *
 * Compare register r with A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero (A == r) like sub but result throw away
 * N - Set
 * H - Set if no borrow from bit 4
 * C - Set for no borrow (Set if A < n)
 */
void Z80::tCP_r(const uint8_t& r) {
    acc = reg.a - r;

    reg.f = check_z(acc & 0xff) | kFlagN;
    reg.f |= (reg.a < r) ? kFlagC : 0;
    reg.f |= ((reg.a > 0xff && r > 0xff) || reg.a < r) ? kFlagH : 0;

    clock += Clock(1);
}

/**
 * INC r
 *
 * Increment register r by one
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Set if carry from bit 3
 * C - Not affected
 */
void Z80::tINC_r(uint8_t& r) {
    bool half_carry = (r & 0x0f) == 0x0f;
    r += 1;

    reg.f = check_z(r) | (reg.f & kFlagC);
    reg.f |= half_carry ? kFlagH : 0;

    clock += Clock(1);
}

/**
 * DEC r
 *
 * Decrement register r by one
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Set
 * H - Set if no borrow from bit 4
 * C - Not affected
 */
void Z80::tDEC_r(uint8_t& r) {
    bool half_carry = (r & 0x18) == 0x10;
    r -= 1;

    reg.f = check_z(r) | (reg.f & kFlagC) | kFlagN;
    reg.f |= half_carry ? kFlagH : 0;

    clock += Clock(1);
}

/**
 * SWAP r
 *
 * Swap upper & lower nibbles of r
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Reset
 */
void Z80::tSWAP_r(uint8_t& r) {
    r = static_cast<uint8_t>(((r << 4) & 0xf0) | ((r >> 4) & 0x0f));
    reg.f = check_z(r);
}

/**
 * INC rr
 *
 * Increment rr register by one
 * rr: BC, DE, HL or SP
 */
void Z80::tINC_rr(uint8_t& rh, uint8_t& rl) {
    uint16_t w = (rh << 8) | rl;
    w += 1;
    split16(w, rh, rl);

    clock += Clock(2);
}

/**
 * DEC rr
 *
 * Decrement rr register by one
 * rr: BC, DE, HL or SP
 */
void Z80::tDEC_rr(uint8_t& rh, uint8_t& rl) {
    uint16_t w = (rh << 8) | rl;
    w -= 1;
    split16(w, rh, rl);

    clock += Clock(2);
}

/**
  * RST n
  *
  * Push present address onto stack and jump to address 0x0000 + n
  */
void Z80::tRST(const uint16_t addr) {
    mmu.write_word(reg.sp, reg.pc);
    reg.sp -= 2;
    reg.pc = addr;
    clock += Clock(8);
}

/**
 * No operation
 */
void Z80::iNOP() {
    clock += Clock(1);
}

void Z80::iLD_A_ADDR_NN() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    reg.a = mmu.read_byte(addr);

    clock += Clock(4);
}

void Z80::iLD_ADDR_NN_A() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    mmu.write_byte(addr, reg.a);

    clock += Clock(4);
}

/**
 * HALT
 *
 * Power down CPU until an interrup occurs
 */
void Z80::iHALT() {
    clock += Clock(1);
}

/*
 * STOP
 *
 * Halt CPU & LCD display until button pressed
 */
void Z80::iSTOP() {
    // TODO: Suspend until interruption
    clock += Clock(1);
}

/**
 * RLCA
 *
 * Rotate A left, Old bit 7 to carry flag
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Contains old bit 7
 */
void Z80::iRLCA() {
    bool has_carry = static_cast<bool>(reg.a & 0x80);

    reg.a <<= 1;
    reg.a += has_carry ? 1 : 0;
    reg.f = check_z(reg.a) | (has_carry ? kFlagC : 0);

    clock += Clock(1);
}

/**
 * RLA
 *
 * Rotate A left through carry
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Contains old bit 7
 */
void Z80::iRLA() {
    bool has_carry = static_cast<bool>(reg.a & 0x80);

    reg.a <<= 1;
    reg.a += (reg.f & kFlagC) ? 1 : 0;
    reg.f = check_z(reg.a) | (has_carry ? kFlagC : 0);

    clock += Clock(1);
}

/**
 * RLCA
 *
 * Rotate A right, Old bit 0 to carry flag
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Contains old bit 0
 */
void Z80::iRRCA() {
    bool has_carry = static_cast<bool>(reg.a & 0x01);

    reg.a >>= 1;
    reg.a += has_carry ? (1 << 7) : 0;
    reg.f = check_z(reg.a) | (has_carry ? kFlagC : 0);

    clock += Clock(1);
}

/**
 * RRA
 *
 * Rotate A right through carry
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Contains old bit 0
 */
void Z80::iRRA() {
    bool has_carry = static_cast<bool>(reg.a & 0x01);

    reg.a <<= 1;
    reg.a += (reg.f & kFlagC) ? (1 << 7) : 0;
    reg.f = check_z(reg.a) | (has_carry ? kFlagC : 0);

    clock += Clock(1);
}

/*
 * CPL
 *
 * Complement A register
 *
 * Flags Affected:
 * Z - Not affected
 * N - Set
 * H - Set
 * C - Not affected
 */
void Z80::iCPL() {
    reg.a = ~reg.a;
    reg.f |= kFlagN | kFlagH;

    clock += Clock(2);
}

void Z80::iADD_HL_SP() {
    uint8_t s = 0;
    uint8_t p = 0;
    split16(reg.sp, s, p);

    tADD_HL_(s, p);

    reg.sp = combine16(s, p);
}

/**
 * SCF
 * Set carry flag
 *
 * Flags Affected:
 * Z - Not affected
 * N - Reset
 * H - Reset
 * C - Set
 */
void Z80::iSCF() {
    reg.f = (reg.f & kFlagZ) | kFlagC;
    clock += Clock(1);
}

/**
 * CCF
 * Complement carry flag
 *
 * Flags Affected:
 * Z - Not affected
 * N - Reset
 * H - Reset
 * C - Complemented
 */
void Z80::iCCF() {
    reg.f = (reg.f & kFlagZ) | (reg.f & kFlagC ? 0 : kFlagC);
    clock += Clock(1);
}

void Z80::iADD_A_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    tADD_A_(value);
}

void Z80::iADD_A_N() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    tADD_A_(value);
}

void Z80::iADD_SP_N() {
    int8_t offset = static_cast<uint8_t>(mmu.read_byte(reg.pc++));
    acc = reg.sp + offset;

    reg.f = check_h(acc) + check_c(acc);
    reg.sp = static_cast<uint16_t>(acc & 0xffff);
    clock += Clock(4);
}

void Z80::iADC_A_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    tADC_A_(value);
}

void Z80::iADC_A_N() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    tADC_A_(value);
}

void Z80::iLD_SP_HL() {
    uint16_t hl = combine16(reg.h, reg.l);
    mmu.write_word(reg.sp, hl);
    clock += Clock(2);
}

void Z80::iLD_HL_SPN() {
    int8_t offset = static_cast<int8_t>(mmu.read_byte(reg.pc++));
    acc = reg.sp + offset;

    reg.f = check_h(acc) | check_c(acc);
    clock += Clock(3);
}

void Z80::iLD_ADDR_HL_N() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(reg.pc++);
    mmu.write_byte(addr, value);
    clock += Clock(3);
}

/**
 * Put value at address FF00 + register C into A
 */
void Z80::iLD_A_OFFSET_ADDR_C() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + reg.c);
    reg.a = mmu.read_byte(addr);

    clock += Clock(2);
}

/**
 * Put A into FF00 + register C
 */
void Z80::iLD_OFFSET_ADDR_C_A() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + reg.c);
    mmu.write_byte(addr, reg.a);

    clock += Clock(2);
}

void Z80::iLD_SP_NN() {
    uint8_t s = mmu.read_byte(reg.pc++);
    uint8_t p = mmu.read_byte(reg.pc++);
    reg.sp = combine16(s, p);

    clock += Clock(3);
}

void Z80::iLD_ADDR_NN_SP() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    mmu.write_word(addr, reg.sp);

    clock += Clock(5);
}

void Z80::iLDH_OFFSET_N_A() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + mmu.read_byte(reg.pc++));
    mmu.write_byte(addr, reg.a);

    clock += Clock(3);
}

void Z80::iLDH_A_OFFSET_N() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + mmu.read_byte(reg.pc++));
    reg.a = mmu.read_byte(addr);

    clock += Clock(3);
}

void Z80::iLDI_A_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    mmu.write_byte(addr, reg.a);
    tINC_rr(reg.h, reg.l);
}

void Z80::iLDI_ADDR_HL_A() {
    uint16_t addr = combine16(reg.h, reg.l);
    reg.a = mmu.read_byte(addr);
    tINC_rr(reg.h, reg.l);
}

void Z80::iLDD_A_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    mmu.write_byte(addr, reg.a);
    tDEC_rr(reg.h, reg.l);
}

void Z80::iLDD_ADDR_HL_A() {
    uint16_t addr = combine16(reg.h, reg.l);
    reg.a = mmu.read_byte(addr);
    tDEC_rr(reg.h, reg.l);
}

void Z80::iINC_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);

    bool half_carry = (value & 0x0f) == 0x0f;
    value += 1;

    reg.f = check_z(value) | (reg.f & kFlagC);
    reg.f |= half_carry ? kFlagH : 0;

    mmu.write_byte(addr, value);
    clock += Clock(3);
}

void Z80::iDEC_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);

    bool half_carry = (value & 0x18) == 0x10;
    value -= 1;

    reg.f = check_z(value) | (reg.f & kFlagC) | kFlagN;
    reg.f |= half_carry ? kFlagH : 0;

    mmu.write_byte(addr, value);
    clock += Clock(3);
}

void Z80::iSUB_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    tSUB(value);
}

void Z80::iSUB_N() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    tSUB(value);
}

void Z80::iSBC_A_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    tSBC_A_(value);
}

void Z80::iSBC_A_N() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    tSBC_A_(value);
}

void Z80::iAND_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    tAND_r(value);
}

void Z80::iAND_N() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    tAND_r(value);
}

void Z80::iOR_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    tOR_r(value);
}

void Z80::iOR_N() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    tOR_r(value);
}

void Z80::iXOR_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    tXOR_r(value);
}

void Z80::iXOR_N() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    tXOR_r(value);
}

void Z80::iCP_ADDR_HL() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    tCP_r(value);
}

void Z80::iCP_N() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);

    tCP_r(value);
}

void Z80::iDI() {
    interruptions_enabled = false;
    clock += Clock(1);
}

void Z80::iEI() {
    interruptions_enabled = true;
    clock += Clock(1);
}

void Z80::iCALL() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint8_t pc_msb = 0;
    uint8_t pc_lsb = 0;
    split16(reg.pc, pc_msb, pc_lsb);

    mmu.write_byte(reg.sp--, pc_lsb);
    mmu.write_byte(reg.sp--, pc_msb);

    reg.pc = combine16(addr_msb, addr_lsb);
    clock += Clock(3);
}

void Z80::iCALL_Z() {
    if ((reg.f & kFlagZ) != 0) {
        iCALL();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::iCALL_NZ() {
    if ((reg.f & kFlagZ) == 0) {
        iCALL();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::iCALL_C() {
    if ((reg.f & kFlagC) != 0) {
        iCALL();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::iCALL_NC() {
    if ((reg.f & kFlagC) == 0) {
        iCALL();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::iRET() {
    reg.pc = mmu.read_word(reg.sp);
    reg.sp += 2;

    clock += Clock(2);
}

void Z80::iRETI() {
    iRET();
    interruptions_enabled = true;
}

void Z80::iRET_Z() {
    if ((reg.f & kFlagZ) != 0) {
        iRET();
    } else {
        clock += Clock(2);
    }
}

void Z80::iRET_NZ() {
    if ((reg.f & kFlagZ) == 0) {
        iRET();
    } else {
        clock += Clock(2);
    }
}

void Z80::iRET_C() {
    if ((reg.f & kFlagC) != 0) {
        iRET();
    } else {
        clock += Clock(2);
    }
}

void Z80::iRET_NC() {
    if ((reg.f & kFlagC) == 0) {
        iRET();
    } else {
        clock += Clock(2);
    }
}

void Z80::iJP() {
    uint8_t l = mmu.read_byte(reg.pc++);
    uint8_t h = mmu.read_byte(reg.pc++);
    reg.pc = combine16(h, l);

    clock += Clock(3);
}

void Z80::iJP_Z() {
    if ((reg.f & kFlagZ) != 0) {
        iJP();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::iJP_NZ() {
    if ((reg.f & kFlagZ) == 0) {
        iJP();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::iJP_C() {
    if ((reg.f & kFlagC) != 0) {
        iJP();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::iJP_NC() {
    if ((reg.f & kFlagC) == 0) {
        iJP();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::iJP_HL() {
    reg.pc = combine16(reg.h, reg.l);
    clock += Clock(1);
}

void Z80::iJR() {
    uint8_t offset = mmu.read_byte(reg.pc++);
    reg.pc += offset;
    clock += Clock(2);
}

void Z80::iJR_Z() {
    if ((reg.f & kFlagZ) != 0) {
        iJR();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void Z80::iJR_NZ() {
    if ((reg.f & kFlagZ) == 0) {
        iJR();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void Z80::iJR_C() {
    if ((reg.f & kFlagC) != 0) {
        iJR();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void Z80::iJR_NC() {
    if ((reg.f & kFlagC) == 0) {
        iJR();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}
