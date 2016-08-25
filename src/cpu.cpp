#include "cpu.hpp"

#include <cstring>

const uint16_t kFlagZ = 1 << 7;
const uint16_t kFlagN = 1 << 6;
const uint16_t kFlagH = 1 << 5;
const uint16_t kFlagC = 1 << 4;

inline uint16_t check_z(int32_t value) {
    return (value == 0) ? kFlagZ : 0x00;
}

inline uint16_t check_n() {
    return kFlagN;
}

inline uint16_t check_h(uint32_t value) {
    return (value & 0x10) == 0x10 ? kFlagH : 0x00;
}

inline uint16_t check_c(int32_t value) {
    return (value > 0xff || value < 0) ? kFlagC : 0x00;
}

inline uint16_t check_h2(uint32_t value) {
    return (value & 0x10) == 0x1000 ? kFlagH : 0x00;
}

inline uint16_t check_c2(int32_t value) {
    return (value > 0xffff || value < 0) ? kFlagC : 0x00;
}

void Z80::reset() {
    std::memset(this, 0, sizeof(Z80));
}

void Z80::iNOP() {
    clock += Clock(1);
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
    reg.a = static_cast<uint8_t>(acc & 0xff);

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
    reg.l = static_cast<uint8_t>(acc & 0xff);

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
    reg.a = static_cast<uint8_t>(acc & 0xff);

    clock += Clock(1);
}


void Z80::iADD_HL_SP() {
    uint8_t s = static_cast<uint8_t>(*reg.sp >> 8);
    uint8_t p = static_cast<uint8_t>(*reg.sp & 0xff);

    tADD_HL_(s, p);

    *reg.sp = static_cast<uint16_t>((s << 8) + p);
}

void Z80::tINC_r(uint8_t& r) {
    r += 1;

    // TODO: Improve carry checking when carry from bit 3
    reg.f = check_z(r) + check_h(r) + (reg.f & kFlagC);

    clock += Clock(1);
}

void Z80::tDEC_r(uint8_t& r) {
    r -= 1;

    // TODO: Improve carry checking when carry borrow bit 4
    reg.f = check_z(r) + check_n() + check_h(r) + (reg.f & kFlagC);

    clock += Clock(1);
}
