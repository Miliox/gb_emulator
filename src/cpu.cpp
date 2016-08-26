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

Z80::Z80() {
    instruction_map.reserve(256);
    cb_instruction_map.reserve(256);

    instruction_map[0x00] = &Z80::iNOP;
    instruction_map[0x01] = &Z80::iNotImplemented; // LD BC,d16
    instruction_map[0x02] = &Z80::iNotImplemented; // LD (BC),A
    instruction_map[0x03] = &Z80::iINC_BC;
    instruction_map[0x04] = &Z80::iINC_B;
    instruction_map[0x05] = &Z80::iDEC_B;
    instruction_map[0x06] = &Z80::iNotImplemented; // LD B,d8
    instruction_map[0x07] = &Z80::iRLCA;
    instruction_map[0x08] = &Z80::iNotImplemented; // LD (a16),SP
    instruction_map[0x09] = &Z80::iADD_HL_BC;
    instruction_map[0x0A] = &Z80::iNotImplemented; // LD A,(BC)
    instruction_map[0x0B] = &Z80::iDEC_BC;
    instruction_map[0x0C] = &Z80::iINC_C;
    instruction_map[0x0D] = &Z80::iDEC_C;
    instruction_map[0x0E] = &Z80::iNotImplemented; // LD C,d8
    instruction_map[0x0F] = &Z80::iRRCA;
    instruction_map[0x10] = &Z80::iNotImplemented; // STOP
    instruction_map[0x11] = &Z80::iNotImplemented; // LD DE,d16
    instruction_map[0x12] = &Z80::iNotImplemented; // LD (DE),A
    instruction_map[0x13] = &Z80::iINC_DE;
    instruction_map[0x14] = &Z80::iINC_D;
    instruction_map[0x15] = &Z80::iDEC_D;
    instruction_map[0x16] = &Z80::iNotImplemented; // LD D,d8
    instruction_map[0x17] = &Z80::iRLA;
    instruction_map[0x18] = &Z80::iNotImplemented; // JR r8
    instruction_map[0x19] = &Z80::iADD_HL_DE;
    instruction_map[0x1A] = &Z80::iNotImplemented; // LD A,(DE)
    instruction_map[0x1B] = &Z80::iDEC_DE;
    instruction_map[0x1C] = &Z80::iINC_E;
    instruction_map[0x1D] = &Z80::iDEC_E;
    instruction_map[0x1E] = &Z80::iNotImplemented; // LD E,d8
    instruction_map[0x1F] = &Z80::iRRA;
    instruction_map[0x20] = &Z80::iNotImplemented; // JR NZ,r8
    instruction_map[0x21] = &Z80::iNotImplemented; // LD HL,d16
    instruction_map[0x22] = &Z80::iNotImplemented; // LD (HL+),A
    instruction_map[0x23] = &Z80::iINC_HL;
    instruction_map[0x24] = &Z80::iINC_H;
    instruction_map[0x25] = &Z80::iDEC_H;
    instruction_map[0x26] = &Z80::iNotImplemented; // LD H,d8
    instruction_map[0x27] = &Z80::iNotImplemented; // DAA
    instruction_map[0x28] = &Z80::iNotImplemented; // JR Z,r8
    instruction_map[0x29] = &Z80::iADD_HL_HL;
    instruction_map[0x2A] = &Z80::iNotImplemented; // LD A,(HL+)
    instruction_map[0x2B] = &Z80::iDEC_HL;
    instruction_map[0x2C] = &Z80::iINC_L;
    instruction_map[0x2D] = &Z80::iDEC_L;
    instruction_map[0x2E] = &Z80::iNotImplemented; // LD L,d8
    instruction_map[0x2F] = &Z80::iCPL;
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
 * Push register pair onto the stack
 * Decrement stack pointer twice
*/
void Z80::tPUSH_rr(const uint8_t& rh, const uint8_t& rl) {
    mmu.write_word(reg.sp, static_cast<uint16_t>((rh << 8) + rl));

    reg.sp -= 2;
    clock += Clock(4);
}

/**
 * Pop two bytes from the stack into register pair
 * Increment stack pointer twice
*/
void Z80::tPOP_rr(uint8_t& rh, uint8_t& rl) {
    uint16_t w = mmu.read_word(reg.sp);

    rh = static_cast<uint8_t>(w >> 8);
    rl = static_cast<uint8_t>(w);

    reg.sp += 2;
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
    reg.h = static_cast<uint8_t>((acc >> 8) & 0xff);
    reg.l = static_cast<uint8_t>(acc);

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

    rh = static_cast<uint8_t>(w >> 8);
    rl = static_cast<uint8_t>(w);

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

    rh = static_cast<uint8_t>(w >> 8);
    rl = static_cast<uint8_t>(w);

    clock += Clock(2);
}

/**
 * No operation
 */
void Z80::iNOP() {
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
    uint8_t s = static_cast<uint8_t>(reg.sp >> 8);
    uint8_t p = static_cast<uint8_t>(reg.sp);

    tADD_HL_(s, p);

    reg.sp = static_cast<uint16_t>((s << 8) + p);
}
