#include "mmu.hpp"

#include "gb_bios.hpp"

#include <iostream>

// Base Address
const uint16_t kAddrInterruptFlag     = 0xffff;
const uint16_t kAddrZeroPageMemory    = 0xff80;
const uint16_t kAddrHardwareIOReg     = 0xff00;
const uint16_t kAddrUnusableMemory    = 0xfea0;
const uint16_t kAddrObjectAttMemory   = 0xfe00;
const uint16_t kAddrEchoRAM           = 0xe000;
const uint16_t kAddrInternalRAMN      = 0xd000;
const uint16_t kAddrInternalRAM0      = 0xc000;
const uint16_t kAddrCartridgeRAM      = 0xa000;
const uint16_t kAddrBGMapData2        = 0x9c00;
const uint16_t kAddrBGMapData1        = 0x9800;
const uint16_t kAddrCharacterRAM      = 0x8000;
const uint16_t kAddrCartridgeROMBankN = 0x4000;
const uint16_t kAddrCartridgeROMBank0 = 0x0150;
const uint16_t kAddrCartridgeHeader   = 0x0100;
const uint16_t kAddrRstInterrVector   = 0x0000;

const uint16_t kSizeZeroPageMemory   = kAddrInterruptFlag - kAddrZeroPageMemory;
const uint16_t kSizeObjectAttMemory  = kAddrUnusableMemory - kAddrObjectAttMemory;
const uint16_t kSizeInternalRAMBank  = kAddrInternalRAMN - kAddrInternalRAM0;
const uint16_t kSizeCharacterRAM     = kAddrBGMapData1 - kAddrCharacterRAM;
const uint16_t kSizeCartridgeROMBank = kAddrCartridgeROMBankN - kAddrCartridgeROMBank0;
const uint16_t kSizeBGMapData        = kAddrCartridgeRAM - kAddrBGMapData1;

void dump_mmu_oper(const char * op, uint16_t offset, uint16_t value);

MMU::MMU() : bios_loaded(true),
    character_ram(new uint8_t[kSizeCharacterRAM]),
    oam_ram(new uint8_t[kSizeObjectAttMemory]),
    zeropage_ram(new uint8_t[kSizeZeroPageMemory]),
    internal_ram(new uint8_t[kSizeInternalRAMBank * 2]),
    bgdata_ram(new uint8_t[kSizeBGMapData]) {

}

uint8_t MMU::read_byte(uint16_t addr) const {
    if (bios_loaded && addr < kGameBoyBiosLength) {
        return kGameBoyBios[addr];
    }

    if (addr >= kAddrZeroPageMemory && addr < kAddrInterruptFlag) {
        uint16_t offset = addr - kAddrZeroPageMemory;
        dump_mmu_oper("zp r", offset, zeropage_ram[offset]);
        return zeropage_ram[offset];
    }

    if (addr >= kAddrCharacterRAM && addr < kAddrBGMapData1) {
        uint16_t offset = addr - kAddrCharacterRAM;
        dump_mmu_oper("ch r", offset, character_ram[offset]);
        return character_ram[offset];
    }

    if (addr >= kAddrInternalRAM0 && addr < kAddrEchoRAM) {
        uint16_t offset = addr - kAddrInternalRAM0;
        dump_mmu_oper("ram r", offset, internal_ram[offset]);
        return internal_ram[offset];
    }

    if (addr >= kAddrObjectAttMemory && addr < kAddrUnusableMemory) {
        uint16_t offset = addr - kAddrObjectAttMemory;
        dump_mmu_oper("oam r", offset, oam_ram[offset]);
        return oam_ram[offset];
    }

    if (addr >= kAddrBGMapData1 && addr < kAddrCartridgeRAM) {
        uint16_t offset = addr - kAddrBGMapData1;
        dump_mmu_oper("bgd r", offset, bgdata_ram[offset]);
        return bgdata_ram[offset];
    }

    return 0;
}

uint16_t MMU::read_word(uint16_t addr) const {
    uint8_t lsb = read_byte(addr);
    uint8_t msb = read_byte(addr + 1);

    return static_cast<uint16_t>((msb << 8) + lsb);
}

void MMU::write_byte(uint16_t addr, uint8_t value) {
    if (addr >= kAddrZeroPageMemory && addr < kAddrInterruptFlag) {
        uint16_t offset = addr - kAddrZeroPageMemory;
        dump_mmu_oper("zp w", offset, value);
        zeropage_ram[offset] = value;
    }

    if (addr >= kAddrCharacterRAM && addr < kAddrBGMapData1) {
        uint16_t offset = addr - kAddrCharacterRAM;
        dump_mmu_oper("ch w", offset, value);
        character_ram[offset] = value;
    }

    if (addr >= kAddrInternalRAM0 && addr < kAddrEchoRAM) {
        uint16_t offset = addr - kAddrInternalRAM0;
        dump_mmu_oper("ram w", offset, value);
        internal_ram[offset] = value;
    }

    if (addr >= kAddrObjectAttMemory && addr < kAddrUnusableMemory) {
        uint16_t offset = addr - kAddrObjectAttMemory;
        dump_mmu_oper("oam w", offset, value);
        oam_ram[offset] = value;
    }

    if (addr >= kAddrBGMapData1 && addr < kAddrCartridgeRAM) {
        uint16_t offset = addr - kAddrBGMapData1;
        dump_mmu_oper("bgd w", offset, value);
        bgdata_ram[offset] = value;
    }
}

void MMU::write_word(uint16_t addr, uint16_t value) {
    uint8_t lsb = static_cast<uint8_t>(value);
    uint8_t msb = static_cast<uint8_t>(value >> 8);

    write_byte(addr, lsb);
    write_byte(addr + 1, msb);
}

void dump_mmu_oper(const char* op, uint16_t offset, uint16_t value) {
    std::cout << std::hex;
    std::cout << op << " @" << offset << " ";
    std::cout << value << "\n";
    std::cout << std::dec;
}
