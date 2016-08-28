#include "mmu.hpp"

#include "gb_bios.hpp"

#include <iostream>
#include <iomanip>

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

const uint16_t kSizeZeroPageMemory   = kAddrInterruptFlag     - kAddrZeroPageMemory;
const uint16_t kSizeObjectAttMemory  = kAddrUnusableMemory    - kAddrObjectAttMemory;
const uint16_t kSizeInternalRAMBank  = kAddrInternalRAMN      - kAddrInternalRAM0;
const uint16_t kSizeCharacterRAM     = kAddrBGMapData1        - kAddrCharacterRAM;
const uint16_t kSizeCartridgeROMBank = kAddrCartridgeROMBankN - kAddrCartridgeROMBank0;
const uint16_t kSizeBGMapData        = kAddrCartridgeRAM      - kAddrBGMapData1;

void dump_mmu_oper(const char * op, uint16_t offset, uint16_t value);
void print_bytes(const std::vector<uint8_t>& data);

GBMMU::GBMMU() : bios_loaded(true),
    character_memory(kSizeCharacterRAM, 0),
    object_attribute_memory(kSizeObjectAttMemory, 0),
    zeropage_memory(kSizeZeroPageMemory, 0),
    internal_ram_memory(kSizeInternalRAMBank * 2, 0),
    bgdata_memory(kSizeBGMapData, 0) {

}

uint8_t GBMMU::read_byte(uint16_t addr) const {
    if (bios_loaded && addr < kGameBoyBiosLength) {
        return kGameBoyBios[addr];
    }

    if (addr >= kAddrZeroPageMemory && addr < kAddrInterruptFlag) {
        uint16_t offset = addr - kAddrZeroPageMemory;
        uint8_t value = zeropage_memory.at(offset);
        dump_mmu_oper("zp r", offset, value);
        return value;
    }

    if (addr >= kAddrCharacterRAM && addr < kAddrBGMapData1) {
        uint16_t offset = addr - kAddrCharacterRAM;
        uint8_t value = character_memory.at(offset);
        dump_mmu_oper("ch r", offset, value);
        return value;
    }

    if (addr >= kAddrInternalRAM0 && addr < kAddrEchoRAM) {
        uint16_t offset = addr - kAddrInternalRAM0;
        uint8_t value = internal_ram_memory.at(offset);
        dump_mmu_oper("ram r", offset, value);
        return value;
    }

    if (addr >= kAddrObjectAttMemory && addr < kAddrUnusableMemory) {
        uint16_t offset = addr - kAddrObjectAttMemory;
        uint8_t value = object_attribute_memory.at(offset);
        dump_mmu_oper("oam r", offset, value);
        return value;
    }

    if (addr >= kAddrBGMapData1 && addr < kAddrCartridgeRAM) {
        uint16_t offset = addr - kAddrBGMapData1;
        uint8_t value = bgdata_memory.at(offset);
        dump_mmu_oper("bgd r", offset, value);
        return value;
    }

    return 0;
}

uint16_t GBMMU::read_word(uint16_t addr) const {
    uint8_t lsb = read_byte(addr);
    uint8_t msb = read_byte(addr + 1);

    return static_cast<uint16_t>((msb << 8) + lsb);
}

void GBMMU::write_byte(uint16_t addr, uint8_t value) {
    if (addr >= kAddrZeroPageMemory && addr < kAddrInterruptFlag) {
        uint16_t offset = addr - kAddrZeroPageMemory;
        dump_mmu_oper("zp w", offset, value);
        zeropage_memory.at(offset) = value;
    }

    if (addr >= kAddrCharacterRAM && addr < kAddrBGMapData1) {
        uint16_t offset = addr - kAddrCharacterRAM;
        dump_mmu_oper("ch w", offset, value);
        character_memory.at(offset) = value;
    }

    if (addr >= kAddrInternalRAM0 && addr < kAddrEchoRAM) {
        uint16_t offset = addr - kAddrInternalRAM0;
        dump_mmu_oper("ram w", offset, value);
        internal_ram_memory.at(offset) = value;
    }

    if (addr >= kAddrObjectAttMemory && addr < kAddrUnusableMemory) {
        uint16_t offset = addr - kAddrObjectAttMemory;
        dump_mmu_oper("oam w", offset, value);
        object_attribute_memory.at(offset) = value;
    }

    if (addr >= kAddrBGMapData1 && addr < kAddrCartridgeRAM) {
        uint16_t offset = addr - kAddrBGMapData1;
        dump_mmu_oper("bgd w", offset, value);
        bgdata_memory.at(offset) = value;
        //print_bytes(bgdata_memory);
    }
}

void GBMMU::write_word(uint16_t addr, uint16_t value) {
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

void print_bytes(const std::vector<uint8_t>& data) {
    std::cout << std::setfill('0');
    for(size_t i = 0; i < data.size(); ++i) {
        std::cout << std::hex << std::setw(2) << (int)data[i];
        std::cout << (((i + 1) % 32 == 0) ? "\n" : " ");
    }
    std::cout << "\n";
}
