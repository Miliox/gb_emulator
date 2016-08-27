#ifndef MMU_HPP
#define MMU_HPP

#include <cstdint>
#include <vector>
#include <memory>

class MMU {
private:
    bool bios_loaded;

    std::unique_ptr<uint8_t[]> character_ram;
    std::unique_ptr<uint8_t[]> oam_ram;
    std::unique_ptr<uint8_t[]> zeropage_ram;
    std::unique_ptr<uint8_t[]> internal_ram;
    std::unique_ptr<uint8_t[]> bgdata_ram;
public:
    MMU();

    uint8_t read_byte(uint16_t addr) const;
    uint16_t read_word(uint16_t addr) const;

    void write_byte(uint16_t addr, uint8_t value);
    void write_word(uint16_t addr, uint16_t value);
};

#endif
