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

    uint8_t read_hwio(uint16_t addr) const;

    void write_hwio(uint16_t addr, uint8_t value);

public:
    GBMMU();
    GBMMU(const GBMMU&) = delete;

    uint8_t  read_byte(uint16_t addr) const;
    uint16_t read_word(uint16_t addr) const;

    void write_byte(uint16_t addr, uint8_t value);
    void write_word(uint16_t addr, uint16_t value);

    void step(uint8_t elapsed_ticks);

    uint8_t hwio_p1;
    uint8_t hwio_sb;
    uint8_t hwio_sc;
    uint8_t hwio_div;
    uint8_t hwio_tima;
    uint8_t hwio_tma;
    uint8_t hwio_tac;
    uint8_t hwio_if;
    uint8_t hwio_nr10;
    uint8_t hwio_nr11;
    uint8_t hwio_nr12;
    uint8_t hwio_nr13;
    uint8_t hwio_nr14;
    uint8_t hwio_nr20;
    uint8_t hwio_nr21;
    uint8_t hwio_nr22;
    uint8_t hwio_nr23;
    uint8_t hwio_nr24;
    uint8_t hwio_nr30;
    uint8_t hwio_nr31;
    uint8_t hwio_nr32;
    uint8_t hwio_nr33;
    uint8_t hwio_nr34;
    uint8_t hwio_nr40;
    uint8_t hwio_nr41;
    uint8_t hwio_nr42;
    uint8_t hwio_nr43;
    uint8_t hwio_nr44;
    uint8_t hwio_nr50;
    uint8_t hwio_nr51;
    uint8_t hwio_nr52;
    uint8_t hwio_wave[16];
    uint8_t hwio_lcdc;
    uint8_t hwio_stat;
    uint8_t hwio_scy;
    uint8_t hwio_scx;
    uint8_t hwio_ly;
    uint8_t hwio_lyc;
    uint8_t hwio_dma;
    uint8_t hwio_bgp;
    uint8_t hwio_obp0;
    uint8_t hwio_obp1;
    uint8_t hwio_wy;
    uint8_t hwio_wx;
    uint8_t hwio_ie;
};

#endif
