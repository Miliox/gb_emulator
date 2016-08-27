#include "mmu.hpp"

#include "gb_bios.hpp"

uint8_t MMU::read_byte(uint16_t addr) {

    //TODO: To be implemented!
    if (bios_loaded && addr < kGameBoyBiosLength) {
        return kGameBoyBios[addr];
    }

    return 0;
}

uint16_t MMU::read_word(uint16_t addr) {
    uint8_t lsb = read_byte(addr);
    uint8_t msb = read_byte(addr + 1);

    return static_cast<uint16_t>((msb << 8) + lsb);
}

void MMU::write_byte(uint16_t addr, uint8_t value) {
    //TODO: To be implemented!
}

void MMU::write_word(uint16_t addr, uint16_t value) {
    uint8_t lsb = static_cast<uint8_t>(value);
    uint8_t msb = static_cast<uint8_t>(value >> 8);

    write_byte(addr, lsb);
    write_byte(addr + 1, msb);
}
