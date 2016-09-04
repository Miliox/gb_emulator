#ifndef MMU_HPP
#define MMU_HPP

#include "clock.hpp"
#include "cartridge.hpp"

#include <cstdint>
#include <fstream>
#include <vector>
#include <memory>

const uint8_t kInterruptionVBlank  = (1 << 0);
const uint8_t kInterruptionLcdStat = (1 << 1);
const uint8_t kInterruptionTimer   = (1 << 2);
const uint8_t kInterruptionSerial  = (1 << 3);
const uint8_t kInterruptionJoypad  = (1 << 4);

const uint8_t kLcdInterruptHBlank = (1 << 3);
const uint8_t kLcdInterruptVBlank = (1 << 4);
const uint8_t kLcdInterruptOAM    = (1 << 5);
const uint8_t kLcdInterruptLineEq = (1 << 6); // Coincidence Flag

enum Interrupt : uint8_t {
    INTERRUPT_VBLANK = kInterruptionVBlank,
    INTERRUPT_LCDC   = kInterruptionLcdStat,
    INTERRUPT_TIMER  = kInterruptionTimer,
    INTERRUPT_SERIAL = kInterruptionSerial,
    INTERRUPT_JOYPAD = kInterruptionJoypad
};

enum LcdcInterrupt : uint8_t {
    LCDC_INTERRUPT_HBLANK = kLcdInterruptHBlank,
    LCDC_INTERRUPT_VBLANK = kLcdInterruptVBlank,
    LCDC_INTERRUPT_OAM    = kLcdInterruptOAM,
    LCDC_INTERRUPT_COINCI = kLcdInterruptLineEq
};

class GBMMU {
private:
    tick_t tick_counter;
    std::vector<uint8_t> vram; // video ram
    std::vector<uint8_t> oram; // object attribute (sprite) ram
    std::vector<uint8_t> hram; // high (zero page) ram
    std::vector<uint8_t> iram; // internal ram

    std::unique_ptr<GBCartridge> cartridge;

    uint8_t read_hwio(uint16_t addr) const;

    void write_hwio(uint16_t addr, uint8_t value);
public:
    GBMMU();
    GBMMU(std::unique_ptr<GBCartridge>&);
    GBMMU(const GBMMU&) = delete;
    ~GBMMU();

    uint8_t  read_byte(uint16_t addr) const;
    uint16_t read_word(uint16_t addr) const;

    void write_byte(uint16_t addr, uint8_t value);
    void write_word(uint16_t addr, uint16_t value);

    void step(tick_t elapsed_ticks);

    void request_interrupt(Interrupt Interrupt);
    void request_lcdc_interrupt(LcdcInterrupt interrupt);

    void check_lcdc_line_coincidence();

    bool bios_loaded;
    bool interrupt_master_enabled;

    void disable_interrupts();
    void enable_interrupts();

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
