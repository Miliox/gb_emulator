#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <cstdint>
#include <fstream>
#include <vector>
#include <string>

enum CartridgeType : uint8_t {
    CARTRIDGE_ROM = 0x00,

    CARTRIDGE_ROM_MBC1 = 0x01,
    CARTRIDGE_ROM_MBC1_RAM = 0x02,
    CARTRIDGE_ROM_MBC1_RAM_BATT = 0x03,

    CARTRIDGE_ROM_MBC2 = 0x05,
    CARTRIDGE_ROM_MBC2_BATT = 0x06,

    CARTRIDGE_ROM_RAM = 0x08,
    CARTRIDGE_ROM_RAM_BATT = 0x09,

    CARTRIDGE_ROM_MMM01 = 0x0B,
    CARTRIDGE_ROM_MMM01_SRAM = 0x0C,
    CARTRIDGE_ROM_MMM01_SRAM_BATT = 0x0D,

    CARTRIDGE_ROM_MBC3_TIMER_BATT = 0x0F,
    CARTRIDGE_ROM_MBC3_TIMER_RAM_BATT = 0x10,
    CARTRIDGE_ROM_MBC3 = 0x11,
    CARTRIDGE_ROM_MBC3_RAM = 0x12,
    CARTRIDGE_ROM_MBC3_RAM_BATT = 0x13,

    CARTRIDGE_ROM_MBC4 = 0x15,
    CARTRIDGE_ROM_MBC4_RAM = 0x16,
    CARTRIDGE_ROM_MBC4_RAM_BATT = 0x17,

    CARTRIDGE_ROM_MBC5 = 0x19,
    CARTRIDGE_ROM_MBC5_RAM = 0x1A,
    CARTRIDGE_ROM_MBC5_RAM_BATT = 0x1B,
    CARTRIDGE_ROM_MBC5_RUMBLE = 0x1C,
    CARTRIDGE_ROM_MBC5_RUMBLE_SRAM = 0x1D,
    CARTRIDGE_ROM_MBC5_RUMBLE_SRAM_BATT = 0x1E

};

class GBCartridge {
private:
    bool has_ram;
    bool has_rom;
    bool has_mcb;

    bool has_battery;
    bool has_mmm01;
    bool has_rumble;
    bool has_timer;

    uint32_t mcb_version;

    uint32_t rom_bank_count;
    uint32_t rom_bank_number;
    uint32_t rom_size;

    uint32_t ram_bank_count;
    uint32_t ram_bank_number;
    uint32_t ram_size;

    bool ram_enabled;

    std::string title;

    bool is_japanese;

    std::vector<uint8_t> rom;
    std::vector<uint8_t> ram;

    bool loaded;

public:
    GBCartridge();

    bool load(const char* filename);
    bool is_loaded() { return loaded; }

    uint8_t read(uint16_t addr) const;

    void dma_read(uint16_t addr, uint16_t length, std::vector<uint8_t>::iterator dst);

    void write(uint16_t addr, uint8_t value);
};

#endif
