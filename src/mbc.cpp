#include "mbc.hpp"

#include <stdexcept>

const uint32_t kRAMBankSize = 0x2000;
const uint32_t kROMBankSize = 0x4000;

MBC::MBC(uint32_t rom_bank_count, uint32_t ram_bank_count) :
    rom_bank_count(rom_bank_count), ram_bank_count(ram_bank_count), rom_bank_number(1), ram_bank_number(0) {

}

MBC1::MBC1(uint32_t rom_bank_count, uint32_t ram_bank_count) : MBC(rom_bank_count, ram_bank_count),
    is_ram_banking_mode(false) {

}

MBC2::MBC2(uint32_t rom_bank_count, uint32_t ram_bank_count) : MBC(rom_bank_count, ram_bank_count) {

}

MBC3::MBC3(uint32_t rom_bank_count, uint32_t ram_bank_count) : MBC(rom_bank_count, ram_bank_count) {

}

MBC4::MBC4(uint32_t rom_bank_count, uint32_t ram_bank_count) : MBC(rom_bank_count, ram_bank_count) {

}

MBC5::MBC5(uint32_t rom_bank_count, uint32_t ram_bank_count) : MBC(rom_bank_count, ram_bank_count) {

}

uint32_t MBC::translate_address(uint16_t addr) {
    return addr;
}

void MBC::write(uint16_t addr, uint8_t value) {

}

uint32_t MBC1::translate_address(uint16_t addr) {
    if (addr <= 0x3fff) {
        // ROM bank #0
        return addr;
    } else if (addr <= 0x7fff) {
        // Switchable ROM Bank
        uint32_t bank = rom_bank_number;
        if (!is_ram_banking_mode) {
            bank += ram_bank_number << 5;
        }

        uint32_t translate_address = addr + (bank - 1) * kROMBankSize;
        return translate_address;
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        // Switchable RAM Bank
        if (!ram_enabled) {
            throw std::runtime_error("trying to access ram while disabled");
        }
        uint32_t bank = (is_ram_banking_mode) ? ram_bank_number : 0;
        return (addr - 0xa000) + bank * kRAMBankSize;
    } else {
        throw std::runtime_error("invalid address");
    }
}

void MBC1::write(uint16_t addr, uint8_t value) {
    if (addr <= 0x1fff) {
        ram_enabled = ((value & 0x0f) == 0x0a) ? true : false;
    } else if (addr >= 2000 && addr <= 0x3fff) {
        uint8_t bank_number_lower = value & 0x1f;
        if (bank_number_lower == 0) {
            bank_number_lower = 1;
        }
        rom_bank_number = (rom_bank_number & 0xe0) | bank_number_lower;
        if (rom_bank_number > rom_bank_count) {
            throw std::runtime_error("invalid rom bank number");
        }
    } else if (addr >= 4000 && addr <= 0x5fff) {
        ram_bank_number = value & 0x3;
        if (ram_bank_number > ram_bank_count) {
            throw std::runtime_error("invalid ram bank number");
        }
    } else if (addr >= 6000 && addr <= 0x7fff) {
        is_ram_banking_mode = value & 0x1;
    }
}

uint32_t MBC2::translate_address(uint16_t addr) {
    throw std::runtime_error("not implemented");
    return 0;
}

void MBC2::write(uint16_t addr, uint8_t value) {
    throw std::runtime_error("not implemented");
}

uint32_t MBC3::translate_address(uint16_t addr) {
    throw std::runtime_error("not implemented");
    return addr;
}

void MBC3::write(uint16_t addr, uint8_t value) {
    throw std::runtime_error("not implemented");
}

uint32_t MBC4::translate_address(uint16_t addr) {
    throw std::runtime_error("not implemented");
    return addr;
}

void MBC4::write(uint16_t addr, uint8_t value) {
    throw std::runtime_error("not implemented");
}

uint32_t MBC5::translate_address(uint16_t addr) {
    throw std::runtime_error("not implemented");
    return addr;
}

void MBC5::write(uint16_t addr, uint8_t value) {
    throw std::runtime_error("not implemented");
}
