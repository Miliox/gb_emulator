#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include <cstdint>
#include <string>

class Instruction {
private:
    uint16_t address;
    uint8_t  opcode;
    uint8_t  arg0;
    uint8_t  arg1;

    static const uint8_t kInstrunctionLength[256];
    static const char*   kInstrunctionNames[256];
public:
    Instruction(uint16_t address, uint8_t opcode, uint8_t arg0, uint8_t arg1)
        : address(address), opcode(opcode), arg0(arg0), arg1(arg1) {
    }

    std::string to_string();
};

#endif
