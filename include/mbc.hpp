#ifndef MBC_HPP
#define MBC_HPP

#include <cstdint>

class MBC {
protected:
    const uint32_t rom_bank_count;
    const uint32_t ram_bank_count;

    uint32_t rom_bank_number;
    uint32_t ram_bank_number;

    bool ram_enabled;
public:
    MBC(uint32_t rom_bank_count, uint32_t ram_bank_count);
    virtual ~MBC() {}

    virtual uint32_t translate_address(uint16_t addr);
    virtual void write(uint16_t addr, uint8_t value);
    virtual bool is_ram_enabled() { return ram_enabled; }
};

class MBC1 : public MBC {
private:
    bool is_ram_banking_mode;

public:
    MBC1(uint32_t rom_bank_count, uint32_t ram_bank_count);
    virtual ~MBC1() override {};

    virtual uint32_t translate_address(uint16_t addr) override;
    virtual void write(uint16_t addr, uint8_t value) override;
};

class MBC2 : public MBC {
public:
    MBC2(uint32_t rom_bank_count, uint32_t ram_bank_count);
    virtual ~MBC2() override {};

    virtual uint32_t translate_address(uint16_t addr) override;
    virtual void write(uint16_t addr, uint8_t value) override;
};

class MBC3 : public MBC {
public:
    MBC3(uint32_t rom_bank_count, uint32_t ram_bank_count);
    virtual ~MBC3() override {};

    virtual uint32_t translate_address(uint16_t addr) override;
    virtual void write(uint16_t addr, uint8_t value) override;
};

class MBC4 : public MBC {
public:
    MBC4(uint32_t rom_bank_count, uint32_t ram_bank_count);
    virtual ~MBC4() override {};

    virtual uint32_t translate_address(uint16_t addr) override;
    virtual void write(uint16_t addr, uint8_t value) override;
};

class MBC5 : public MBC {
public:
    MBC5(uint32_t rom_bank_count, uint32_t ram_bank_count);
    virtual ~MBC5() override {};

    virtual uint32_t translate_address(uint16_t addr) override;
    virtual void write(uint16_t addr, uint8_t value) override;
};

#endif
