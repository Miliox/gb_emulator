#include "cartridge.hpp"

#include <iostream>
#include <stdexcept>

const int kAddrCartridgeGameTitle = 0x134;
const int kAddrCartridgeSGBIndicator = 0x146;
const int kAddrCartridgeType = 0x147;
const int kAddrCartridgeROMSize = 0x148;
const int kAddrCartridgeRAMSize = 0x149;
const int kAddrCartridgeDestinationCode = 0x14a;

bool contains_mbc(uint8_t cartridge_type);
bool contains_rom(uint8_t cartridge_type);
bool contains_ram(uint8_t cartridge_type);
bool contains_battery(uint8_t cartridge_type);
bool contains_rumble(uint8_t cartridge_type);
bool contains_timer(uint8_t cartridge_type);
bool contains_mmm01(uint8_t cartridge_type);

uint32_t get_mbc_version(uint8_t cartridge_type);
uint32_t get_ram_bank_count(uint8_t ram_type);
uint32_t get_rom_bank_count(uint8_t rom_type);
uint32_t get_ram_size(uint8_t ram_type);
uint32_t get_rom_size(uint8_t rom_type);

GBCartridge::GBCartridge() :
    has_ram(false), has_rom(false), has_mbc(false),
    has_battery(false), has_mmm01(false), has_rumble(false), has_timer(false),
    mbc(new MBC(0, 0)), mbc_version(0),
    rom_size(0), ram_size(0),
    rom(), ram(),
    loaded(false),
    title(), is_japanese(false) {

}

bool GBCartridge::load(const char* filename) {
    has_ram = false;
    has_rom = false;
    has_mbc = false;
    has_battery = false;
    has_mmm01 = false;
    has_rumble = false;
    has_timer = false;

    mbc_version = 0;

    rom_size = 0;
    ram_size = 0;

    title.clear();
    is_japanese = false;

    rom.clear();
    ram.clear();

    loaded = false;

    std::fstream file(filename, std::fstream::in);
    if (file.is_open()) {
        // load title
        char title_str[12];
        std::memset(title_str, 0, sizeof(title_str));
        file.seekg(kAddrCartridgeGameTitle, file.beg);
        file.read(title_str, sizeof(title_str));
        title_str[sizeof(title_str) - 1] = '\0';
        title = std::string(title_str);

        // load location
        file.seekg(kAddrCartridgeType, file.beg);
        is_japanese = (file.get() == 0) ? true : false;

        // load type
        file.seekg(kAddrCartridgeType, file.beg);
        uint8_t cartridge_type = file.get();
        has_rom = contains_rom(cartridge_type);
        has_ram = contains_ram(cartridge_type);
        has_mbc = contains_mbc(cartridge_type);
        has_mmm01 = contains_mmm01(cartridge_type);
        has_timer = contains_timer(cartridge_type);
        has_rumble = contains_rumble(cartridge_type);
        has_battery = contains_battery(cartridge_type);

        uint32_t rom_bank_count = 0;
        if (has_rom) {
            file.seekg(kAddrCartridgeROMSize, file.beg);
            uint8_t rom_type = file.get();
            rom_bank_count = get_rom_bank_count(rom_type);
            rom_size = get_rom_size(rom_type);
        }

        uint32_t ram_bank_count = 0;
        if (has_ram) {
            file.seekg(kAddrCartridgeRAMSize, file.beg);
            uint8_t ram_type = file.get();
            ram_bank_count = get_ram_bank_count(ram_type);
            ram_size = get_ram_size(ram_type);
        }

        if (has_mbc) {
            mbc_version = get_mbc_version(cartridge_type);

            if (mbc_version == 1) {
                mbc.reset(new MBC1(rom_bank_count, ram_bank_count));
            } else if (mbc_version == 2) {
                mbc.reset(new MBC2(rom_bank_count, ram_bank_count));
            } else if (mbc_version == 3) {
                mbc.reset(new MBC3(rom_bank_count, ram_bank_count));
            } else if (mbc_version == 4) {
                mbc.reset(new MBC4(rom_bank_count, ram_bank_count));
            } else if (mbc_version == 5) {
                mbc.reset(new MBC5(rom_bank_count, ram_bank_count));
            }

        }

        // load ram
        rom.resize(rom_size, 0);
        file.seekg(0, file.beg);
        file.read(reinterpret_cast<char*>(&rom[0]), rom_size);

        // allocate ram
        ram.resize(ram_size, 0);

        loaded = true;
    }
    file.close();

    return loaded;
}

uint8_t GBCartridge::read(uint16_t addr) const {
    if (has_rom && (addr >= 0x0000 && addr <= 0x7fff)) {
        return rom.at(mbc->translate_address(addr));
    }

    if (has_ram && (addr >= 0xa000 && addr <= 0xbfff)) {
        return ram.at(mbc->translate_address(addr));
    }

    return 0;
}

void GBCartridge::write(uint16_t addr, uint8_t value) {
    if (has_mbc && (addr >= 0x0000 && addr <= 0x7fff)) {
        mbc->write(addr, value);
    }

    else if (has_ram && mbc->is_ram_enabled() && (addr >= 0xa000 && addr <= 0xbfff)) {
        ram.at(mbc->translate_address(addr)) = value;
    }
}

void GBCartridge::dma_read(uint16_t addr, uint16_t length, std::vector<uint8_t>::iterator dst) {
    if (has_rom && (addr >= 0x0000 && addr <= 0x7fff)) {
        std::copy(
            rom.begin() + addr,
            rom.begin() + addr + length,
            dst);
    }
}

uint32_t get_rom_bank_count(uint8_t rom_type) {
    switch (rom_type) {
        case 0x0: // 32kB, 2 banks (no switch)
            return 2;
        case 0x1: // 64kB, 4 banks
            return 4;
        case 0x2: // 128kB, 8 banks
            return 8;
        case 0x3: // 256kB, 16 banks
            return 16;
        case 0x4: // 512kB, 32 banks
            return 32;
        case 0x5: // 1MB, 64 banks
            return 64;
        case 0x6: // 2MB, 128 banks
            return 128;
        case 0x7: // 4MB, 256 banks
            return 256;
        case 0x52: // 1.1MB, 72 banks
            return 72;
        case 0x53: // 1.2MB, 80 banks
            return 80;
        case 0x54: // 1.5MB, 96 Banks
            return 96;
        default: // ???, At least 2
            return 0;
    }
}

uint32_t kSizeCartridgeBank = 0x4000;

uint32_t get_rom_size(uint8_t rom_type) {
    return kSizeCartridgeBank * get_rom_bank_count(rom_type);
}

uint32_t get_ram_bank_count(uint8_t ram_type) {
    switch (ram_type) {
        case 0: // None
            return 0;
        case 1: // 2kB, 1 bank
            return 1;
        case 2: // 8kB, 1 bank
            return 1;
        case 3: // 32kB, 4 banks
            return 4;
        case 4: //128kB, 16 banks
            return 16;
        default:
            return 0;
    }
}

uint32_t get_ram_size(uint8_t ram_type) {
    switch (ram_type) {
        case 0: // None
            return 0;
        case 1: // 2kB, 1 bank
            return 2 * 1024;
        case 2: // 8kB, 1 bank
            return 8 * 1024;
        case 3: // 32kB, 4 banks
            return 32 * 1024;
        case 4: //128kB, 16 banks
            return 128 * 1024;
        default:
            return 0;
    }
}

bool contains_rom(uint8_t cartridge_type) {
    switch (cartridge_type) {
        case CARTRIDGE_ROM:
        case CARTRIDGE_ROM_MBC1:
        case CARTRIDGE_ROM_MBC1_RAM:
        case CARTRIDGE_ROM_MBC1_RAM_BATT:
        case CARTRIDGE_ROM_MBC2:
        case CARTRIDGE_ROM_MBC2_BATT:
        case CARTRIDGE_ROM_RAM:
        case CARTRIDGE_ROM_RAM_BATT:
        case CARTRIDGE_ROM_MMM01:
        case CARTRIDGE_ROM_MMM01_SRAM:
        case CARTRIDGE_ROM_MMM01_SRAM_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_RAM_BATT:
        case CARTRIDGE_ROM_MBC3:
        case CARTRIDGE_ROM_MBC3_RAM:
        case CARTRIDGE_ROM_MBC3_RAM_BATT:
        case CARTRIDGE_ROM_MBC4:
        case CARTRIDGE_ROM_MBC4_RAM:
        case CARTRIDGE_ROM_MBC4_RAM_BATT:
        case CARTRIDGE_ROM_MBC5:
        case CARTRIDGE_ROM_MBC5_RAM:
        case CARTRIDGE_ROM_MBC5_RAM_BATT:
        case CARTRIDGE_ROM_MBC5_RUMBLE:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM_BATT:
            return true;
        default:
            return false;
    }
}

bool contains_ram(uint8_t cartridge_type) {
    switch (cartridge_type) {
        case CARTRIDGE_ROM_MBC1_RAM:
        case CARTRIDGE_ROM_MBC1_RAM_BATT:
        case CARTRIDGE_ROM_RAM:
        case CARTRIDGE_ROM_RAM_BATT:
        case CARTRIDGE_ROM_MMM01_SRAM:
        case CARTRIDGE_ROM_MMM01_SRAM_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_RAM_BATT:
        case CARTRIDGE_ROM_MBC3_RAM:
        case CARTRIDGE_ROM_MBC3_RAM_BATT:
        case CARTRIDGE_ROM_MBC4_RAM:
        case CARTRIDGE_ROM_MBC4_RAM_BATT:
        case CARTRIDGE_ROM_MBC5_RAM:
        case CARTRIDGE_ROM_MBC5_RAM_BATT:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM_BATT:
            return true;
        default:
            return false;
    }
}

bool contains_rumble(uint8_t cartridge_type) {
    switch (cartridge_type) {
        case CARTRIDGE_ROM_MBC5_RUMBLE:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM_BATT:
            return true;
        default:
            return false;
    }
}

bool contains_battery(uint8_t cartridge_type) {
    switch (cartridge_type) {
        case CARTRIDGE_ROM_MBC1_RAM_BATT:
        case CARTRIDGE_ROM_MBC2_BATT:
        case CARTRIDGE_ROM_RAM_BATT:
        case CARTRIDGE_ROM_MMM01_SRAM_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_RAM_BATT:
        case CARTRIDGE_ROM_MBC3_RAM_BATT:
        case CARTRIDGE_ROM_MBC4_RAM_BATT:
        case CARTRIDGE_ROM_MBC5_RAM_BATT:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM_BATT:
            return true;
        default:
            return false;
    }
}

bool contains_timer(uint8_t cartridge_type) {
    switch (cartridge_type) {
        case CARTRIDGE_ROM_MBC3_TIMER_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_RAM_BATT:
            return true;
        default:
            return false;
    }
}

bool contains_mmm01(uint8_t cartridge_type) {
    switch (cartridge_type) {
        case CARTRIDGE_ROM_MMM01:
        case CARTRIDGE_ROM_MMM01_SRAM:
        case CARTRIDGE_ROM_MMM01_SRAM_BATT:
            return true;
        default:
            return false;
    }
}

bool contains_mbc(uint8_t cartridge_type) {
    switch (cartridge_type) {
        case CARTRIDGE_ROM_MBC1:
        case CARTRIDGE_ROM_MBC1_RAM:
        case CARTRIDGE_ROM_MBC1_RAM_BATT:
        case CARTRIDGE_ROM_MBC2:
        case CARTRIDGE_ROM_MBC2_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_RAM_BATT:
        case CARTRIDGE_ROM_MBC3:
        case CARTRIDGE_ROM_MBC3_RAM:
        case CARTRIDGE_ROM_MBC3_RAM_BATT:
        case CARTRIDGE_ROM_MBC4:
        case CARTRIDGE_ROM_MBC4_RAM:
        case CARTRIDGE_ROM_MBC4_RAM_BATT:
        case CARTRIDGE_ROM_MBC5:
        case CARTRIDGE_ROM_MBC5_RAM:
        case CARTRIDGE_ROM_MBC5_RAM_BATT:
        case CARTRIDGE_ROM_MBC5_RUMBLE:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM_BATT:
            return true;
        default:
            return false;
    }
}

uint32_t get_mbc_version(uint8_t cartridge_type) {
    switch (cartridge_type) {
        case CARTRIDGE_ROM_MBC1:
        case CARTRIDGE_ROM_MBC1_RAM:
        case CARTRIDGE_ROM_MBC1_RAM_BATT:
            return 1;
        case CARTRIDGE_ROM_MBC2:
        case CARTRIDGE_ROM_MBC2_BATT:
            return 2;
        case CARTRIDGE_ROM_MBC3_TIMER_BATT:
        case CARTRIDGE_ROM_MBC3_TIMER_RAM_BATT:
        case CARTRIDGE_ROM_MBC3:
        case CARTRIDGE_ROM_MBC3_RAM:
        case CARTRIDGE_ROM_MBC3_RAM_BATT:
            return 3;
        case CARTRIDGE_ROM_MBC4:
        case CARTRIDGE_ROM_MBC4_RAM:
        case CARTRIDGE_ROM_MBC4_RAM_BATT:
            return 4;
        case CARTRIDGE_ROM_MBC5:
        case CARTRIDGE_ROM_MBC5_RAM:
        case CARTRIDGE_ROM_MBC5_RAM_BATT:
        case CARTRIDGE_ROM_MBC5_RUMBLE:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM:
        case CARTRIDGE_ROM_MBC5_RUMBLE_SRAM_BATT:
            return 5;
        default:
            return 0;
    }
}
