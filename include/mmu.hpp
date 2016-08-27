#ifndef MMU_HPP
#define MMU_HPP

#include <cstdint>
#include <vector>
#include <memory>

class GBMMU {
private:
    bool bios_loaded;

    std::vector<uint8_t> character_memory;
    std::vector<uint8_t> object_attribute_memory;
    std::vector<uint8_t> zeropage_memory;
    std::vector<uint8_t> internal_ram_memory;
    std::vector<uint8_t> bgdata_memory;

public:
    GBMMU();
    GBMMU(const GBMMU&) = delete;

    uint8_t  read_byte(uint16_t addr) const;
    uint16_t read_word(uint16_t addr) const;

    void write_byte(uint16_t addr, uint8_t value);
    void write_word(uint16_t addr, uint16_t value);
};

#endif
