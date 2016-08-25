#ifndef MMU_HPP
#define MMU_HPP

#include <cstdint>

class MMU {
public:
    uint8_t read_byte(uint16_t addr);
    uint16_t read_word(uint16_t addr);

    void write_byte(uint16_t addr, uint8_t value);
    void write_byte(uint16_t addr, uint16_t value);
};

#endif
