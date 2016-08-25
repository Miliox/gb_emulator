#include "cpu.hpp"

#include <cstring>

inline uint16_t check_zero(int32_t value) {
    return (value == 0) ? 0x80 : 0x00;
}

inline uint16_t check_negative() {
    return 0x40;
}

inline uint16_t check_carry(int32_t value) {
    return (value > 255 || value < 0) ? 0x10 : 0x00;
}

void Z80::reset() {
    std::memset(this, 0, sizeof(Z80));
}

void Z80::iNOP() {
    clock += Clock(1);
}

void Z80::iADD_A_E() {
    acc = reg.a + reg.e;
    reg.f = check_zero(acc & 0xff) + check_carry(acc);
    reg.a = static_cast<uint8_t>(acc & 0xff);

    clock += Clock(1);
}
