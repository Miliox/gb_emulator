#include "mmu.hpp"

#include "gb_bios.hpp"

#include <iostream>
#include <iomanip>
#include <cassert>

// Base Addresses
const uint16_t kAddrInterruptFlag     = 0xffff;
/*
const uint16_t kAddrZeroPageMemory    = 0xff80;
const uint16_t kAddrHardwareIOReg     = 0xff00;
const uint16_t kAddrUnusableMemory    = 0xfea0;
const uint16_t kAddrObjectAttMemory   = 0xfe00;
const uint16_t kAddrEchoRAM           = 0xe000;
const uint16_t kAddrInternalRAMN      = 0xd000;
const uint16_t kAddrInternalRAM0      = 0xc000;
const uint16_t kAddrCartridgeRAM      = 0xa000;
const uint16_t kAddrBGMapData2        = 0x9c00;
const uint16_t kAddrBGMapData1        = 0x9800;
const uint16_t kAddrCharacterRAM      = 0x8000;
const uint16_t kAddrCartridgeROMBankN = 0x4000;
const uint16_t kAddrCartridgeROMBank0 = 0x0150;
const uint16_t kAddrCartridgeHeader   = 0x0100;
const uint16_t kAddrRstInterrVector   = 0x0000;
*/

// Hardware IO Addresses
const uint16_t kAddrP1   = 0xff00;
const uint16_t kAddrSB   = 0xff01;
const uint16_t kAddrSC   = 0xff02;
const uint16_t kAddrDIV  = 0xff04;
const uint16_t kAddrTIMA = 0xff05;
const uint16_t kAddrTMA  = 0xff06;
const uint16_t kAddrTAC  = 0xff07;
const uint16_t kAddrIF   = 0xff0f;
const uint16_t kAddrNR10 = 0xff10;
const uint16_t kAddrNR11 = 0xff11;
const uint16_t kAddrNR12 = 0xff12;
const uint16_t kAddrNR13 = 0xff13;
const uint16_t kAddrNR14 = 0xff14;
const uint16_t kAddrNR21 = 0xff16;
const uint16_t kAddrNR22 = 0xff17;
const uint16_t kAddrNR23 = 0xff18;
const uint16_t kAddrNR24 = 0xff19;
const uint16_t kAddrNR30 = 0xff1a;
const uint16_t kAddrNR31 = 0xff1b;
const uint16_t kAddrNR32 = 0xff1c;
const uint16_t kAddrNR33 = 0xff1d;
const uint16_t kAddrNR34 = 0xff1e;
const uint16_t kAddrNR41 = 0xff20;
const uint16_t kAddrNR42 = 0xff21;
const uint16_t kAddrNR43 = 0xff22;
const uint16_t kAddrNR44 = 0xff23;
const uint16_t kAddrNR50 = 0xff24;
const uint16_t kAddrNR51 = 0xff25;
const uint16_t kAddrNR52 = 0xff26;
const uint16_t kAddrWave = 0xff30;
const uint16_t kAddrLCDC = 0xff40;
const uint16_t kAddrSTAT = 0xff41;
const uint16_t kAddrSCY  = 0xff42;
const uint16_t kAddrSCX  = 0xff43;
const uint16_t kAddrLY   = 0xff44;
const uint16_t kAddrLYC  = 0xff45;
const uint16_t kAddrDMA  = 0xff46;
const uint16_t kAddrBGP  = 0xff47;
const uint16_t kAddrOBP0 = 0xff48;
const uint16_t kAddrOBP1 = 0xff49;
const uint16_t kAddrWY   = 0xff4a;
const uint16_t kAddrWX   = 0xff4b;

const uint16_t kAddrUnloadBIOS = 0xff50;

const uint16_t kAddrHRAM = 0xff80;
const uint16_t kAddrHWIO = 0xff00;
const uint16_t kAddrORAM = 0xfe00;
const uint16_t kAddrIRAM = 0xc000;
const uint16_t kAddrVRAM = 0x8000;
const uint16_t kAddrCROM = 0x0000;

const uint16_t kSizeHRAM = (0xfffe - kAddrHRAM) + 1;
const uint16_t kSizeHWIO = (0xff80 - kAddrHWIO) + 1;
const uint16_t kSizeORAM = (0xfe9f - kAddrORAM) + 1;
const uint16_t kSizeIRAM = (0xdfff - kAddrIRAM) + 1;
const uint16_t kSizeVRAM = (0x9fff - kAddrVRAM) + 1;
const uint16_t kSizeCROM = (0x7fff - kAddrCROM) + 1;

const uint16_t kSizeCartridgeBank = 0x4000;

void dump_mmu_oper(const char * op, uint16_t offset, uint16_t value);

GBMMU::GBMMU() :
    tick_counter(0),
    vram(kSizeVRAM, 0),
    oram(kSizeORAM, 0),
    hram(kSizeHRAM, 0),
    iram(kSizeIRAM, 0),
    bios_loaded(true),
    interrupt_master_enabled(false) {

    hwio_p1 = 0;
    hwio_sb = 0;
    hwio_sc = 0;
    hwio_div = 0;
    hwio_tima = 0;
    hwio_tma = 0;
    hwio_tac = 0;
    hwio_if = 0;
    hwio_nr10 = 0;
    hwio_nr11 = 0;
    hwio_nr12 = 0;
    hwio_nr13 = 0;
    hwio_nr14 = 0;
    hwio_nr20 = 0;
    hwio_nr21 = 0;
    hwio_nr22 = 0;
    hwio_nr23 = 0;
    hwio_nr24 = 0;
    hwio_nr30 = 0;
    hwio_nr31 = 0;
    hwio_nr32 = 0;
    hwio_nr33 = 0;
    hwio_nr34 = 0;
    hwio_nr40 = 0;
    hwio_nr41 = 0;
    hwio_nr42 = 0;
    hwio_nr43 = 0;
    hwio_nr44 = 0;
    hwio_nr50 = 0;
    hwio_nr51 = 0;
    hwio_nr52 = 0;
    std::memset(hwio_wave, 0, sizeof(hwio_wave));
    hwio_lcdc = 0;
    hwio_stat = 0;
    hwio_scy = 0;
    hwio_scx = 0;
    hwio_ly = 0;
    hwio_lyc = 0;
    hwio_dma = 0;
    hwio_bgp = 0;
    hwio_obp0 = 0;
    hwio_obp1 = 0;
    hwio_wy = 0;
    hwio_wx = 0;
    hwio_ie = 0;

    joypad_state = 0;
}

GBMMU::GBMMU(std::unique_ptr<GBCartridge>& cartridge) : GBMMU() {
    // take ownership
    this->cartridge.reset(cartridge.release());
}

GBMMU::~GBMMU() {

}

inline uint8_t read(uint16_t addr, uint16_t base, const std::vector<uint8_t>& memory) {
    assert(addr >= base);
    return memory.at(addr - base);
}

uint8_t GBMMU::read_byte(uint16_t addr) const {
    if (bios_loaded && addr < kGameBoyBiosLength) {
        return kGameBoyBios[addr];
    }

    if (addr < 0x8000) {
        uint8_t value = cartridge->read(addr);
        //dump_mmu_oper("r cart", addr, value);
        return value;
    }

    const int len = 4;
    //const char* mem_name[len] = {r vram", "r iram", "r oram", "r hram"};
    const uint16_t mem_addr[len] = {kAddrVRAM, kAddrIRAM, kAddrORAM, kAddrHRAM};
    const uint16_t mem_size[len] = {kSizeVRAM, kSizeIRAM, kSizeORAM, kSizeHRAM};
    const std::vector<uint8_t>* mem[len] = {&vram, &iram, &oram, &hram};

    for (int i = 0; i < len; i++) {
        if (addr >= mem_addr[i] && addr < (mem_addr[i] + mem_size[i])) {
            uint8_t value = read(addr, mem_addr[i], *mem[i]);
            //dump_mmu_oper(mem_name[i], addr, value);
            return value;
        }
    }

    if (addr == kAddrInterruptFlag) {
        //dump_mmu_oper("r ie", addr, value);
        return hwio_ie;
    } else if (addr >= kAddrHWIO && addr < (kAddrHWIO + kSizeHWIO)) {
        uint8_t value = read_hwio(addr);
        //dump_mmu_oper("r hw", addr, value);
        return value;
    } else {
        return 0;
    }
}

uint16_t GBMMU::read_word(uint16_t addr) const {
    uint8_t lsb = read_byte(addr);
    uint8_t msb = read_byte(addr + 1);
    return static_cast<uint16_t>((msb << 8) + lsb);
}

inline void write(uint8_t value, uint16_t addr, uint16_t base,
    std::vector<uint8_t>& memory) {
    assert(addr >= base);
    memory.at(addr - base) = value;
}

void GBMMU::write_byte(uint16_t addr, uint8_t value) {
    if (addr < 0x8000) {
        cartridge->write(addr, value);
        //dump_mmu_oper("w cart", addr, value);
        return;
    }

    const int len = 4;
    //const char* mem_name[len] = {"w vram", "w iram", "w oram", "w hram"};
    const uint16_t mem_addr[len] = {kAddrVRAM, kAddrIRAM, kAddrORAM, kAddrHRAM};
    const uint16_t mem_size[len] = {kSizeVRAM, kSizeIRAM, kSizeORAM, kSizeHRAM};
    std::vector<uint8_t>* mem[len] = {&vram, &iram, &oram, &hram};

    for (int i = 0; i < len; i++) {
        if (addr >= mem_addr[i] && addr < (mem_addr[i] + mem_size[i])) {
            write(value, addr, mem_addr[i], *mem[i]);
            //dump_mmu_oper(mem_name[i], addr, value);
            return;
        }
    }

    if (addr == kAddrInterruptFlag) {
        //dump_mmu_oper("w ie", addr, value);
        hwio_ie = value;
        hwio_if &= hwio_ie;
    } else if (addr >= kAddrHWIO && addr < (kAddrHWIO + kSizeHWIO)) {
        write_hwio(addr, value);
        //dump_mmu_oper("w hw", addr, value);
    } else {
        //std::cout << "w ign @" << addr << ": " << value << "\n";
    }
}

void GBMMU::write_word(uint16_t addr, uint16_t value) {
    uint8_t lsb = static_cast<uint8_t>(value);
    uint8_t msb = static_cast<uint8_t>(value >> 8);

    write_byte(addr, lsb);
    write_byte(addr + 1, msb);
}

const tick_t kCounterFrequencies[4] = {4096, 262144, 65536, 16384};
const tick_t kCounterPeriod[4] = {
    kTicksPerSecond / kCounterFrequencies[0],
    kTicksPerSecond / kCounterFrequencies[1],
    kTicksPerSecond / kCounterFrequencies[2],
    kTicksPerSecond / kCounterFrequencies[3]};

void GBMMU::step(tick_t elapsed_ticks) {
    if (hwio_tac & 0x04) {
        uint8_t clock_selected = hwio_tac & 0x03;

        tick_counter += elapsed_ticks;
        if (tick_counter >= kCounterPeriod[clock_selected]) {
            tick_counter -= kCounterPeriod[clock_selected];
            hwio_tima += 1;

            if (hwio_tima == 0) {
                request_interrupt(INTERRUPT_TIMER);
            }
        }
    } else {
        tick_counter = 0;
    }
}

void GBMMU::disable_interrupts() {
    interrupt_master_enabled = false;
}

void GBMMU::enable_interrupts() {
    interrupt_master_enabled = true;
}

void GBMMU::request_interrupt(Interrupt interrupt) {
    hwio_if |= interrupt;
}

void GBMMU::request_lcdc_interrupt(LcdcInterrupt interrupt) {
    if (hwio_stat & interrupt) {
        request_interrupt(INTERRUPT_LCDC);
    }
}

void GBMMU::set_joypad_state(uint8_t state) {
    if (joypad_state != state) {
        request_interrupt(INTERRUPT_JOYPAD);
    }
    joypad_state = state;
    update_p1();
}

void GBMMU::update_p1() {
    switch ((hwio_p1 >> 4) & 0x3) {
        case 0:
            hwio_p1 = (hwio_p1 & 0xf0);
            break;
        case 1:
            hwio_p1 = (hwio_p1 & 0xf0) | (joypad_state & 0x0f);
            break;
        case 2:
            hwio_p1 = (hwio_p1 & 0xf0) | (joypad_state >> 4);
            break;
        case 3:
            hwio_p1 = (hwio_p1 & 0xf0) | ((joypad_state & 0x0f) | (joypad_state >> 4));
            break;
    }
}

uint8_t GBMMU::read_hwio(uint16_t addr) const {
    switch (addr) {
        case kAddrP1:
            return hwio_p1;
        case kAddrSB:
            return hwio_sb;
        case kAddrSC:
            return hwio_sc;
        case kAddrDIV:
            return hwio_div;
        case kAddrTIMA:
            return hwio_tima;
        case kAddrTMA:
            return hwio_tma;
        case kAddrTAC:
            return hwio_tac & 0x07;
        case kAddrIF:
            return hwio_if;
        case kAddrNR10:
            return hwio_nr10;
        case kAddrNR11:
            return hwio_nr11 & 0xC0;
        case kAddrNR12:
            return hwio_nr12;
        case kAddrNR13:
            return 0; // write only
        case kAddrNR14:
            return hwio_nr14 & 0x40;
        case kAddrNR21:
            return hwio_nr21 & 0xC0;
        case kAddrNR22:
            return hwio_nr22;
        case kAddrNR23:
            return 0; // write only
        case kAddrNR24:
            return hwio_nr24 & 0x40;
        case kAddrNR30:
            return hwio_nr30;
        case kAddrNR31:
            return hwio_nr31;
        case kAddrNR32:
            return hwio_nr32;
        case kAddrNR33:
            return 0; // write only
        case kAddrNR34:
            return hwio_nr34 & 0x40;
        case kAddrNR41:
            return hwio_nr41;
        case kAddrNR42:
            return hwio_nr42;
        case kAddrNR43:
            return hwio_nr43;
        case kAddrNR44:
            return hwio_nr44 & 0x40;
        case kAddrNR50:
            return hwio_nr50;
        case kAddrNR51:
            return hwio_nr51;
        case kAddrNR52:
            return hwio_nr52;
        case kAddrWave + 0x0:
        case kAddrWave + 0x1:
        case kAddrWave + 0x2:
        case kAddrWave + 0x3:
        case kAddrWave + 0x4:
        case kAddrWave + 0x5:
        case kAddrWave + 0x6:
        case kAddrWave + 0x7:
        case kAddrWave + 0x8:
        case kAddrWave + 0x9:
        case kAddrWave + 0xa:
        case kAddrWave + 0xb:
        case kAddrWave + 0xc:
        case kAddrWave + 0xd:
        case kAddrWave + 0xe:
        case kAddrWave + 0xf:
            return hwio_wave[addr - kAddrWave];
        case kAddrLCDC:
            return hwio_lcdc;
        case kAddrSTAT:
            return hwio_stat;
        case kAddrSCY:
            return hwio_scy;
        case kAddrSCX:
            return hwio_scx;
        case kAddrLY:
            return hwio_ly;
        case kAddrLYC:
            return hwio_lyc;
        case kAddrDMA:
            return 0; // write only
        case kAddrBGP:
            return hwio_bgp;
        case kAddrOBP0:
            return hwio_obp0;
        case kAddrOBP1:
            return hwio_obp1;
        case kAddrWY:
            return hwio_wy;
        case kAddrWX:
            return hwio_wx;
        default:
            //std::cout << "ign r @" << addr << "\n";
            return 0;
    }
}

void GBMMU::write_hwio(uint16_t addr,  uint8_t value) {
    switch (addr) {
        case kAddrP1:
            hwio_p1 = (hwio_p1 & 0x0f) | (value & 0x30);
            update_p1();
            break;
        case kAddrSB:
            hwio_sb = value;
            break;
        case kAddrSC:
            hwio_sc = value & 0x81;
            break;
        case kAddrDIV:
            hwio_div = 0;
            break;
        case kAddrTIMA:
            hwio_tima = value;
            break;
        case kAddrTMA:
            hwio_tma = value;
            break;
        case kAddrTAC:
            hwio_tac = value & 0x07;
            break;
        case kAddrIF:
            hwio_if = value;
            break;
        case kAddrNR10:
            hwio_nr10 = value & 0x7f;
            break;
        case kAddrNR11:
            hwio_nr11 = value;
            break;
        case kAddrNR12:
            hwio_nr12 = value;
            break;
        case kAddrNR13:
            hwio_nr13 = value;
            break;
        case kAddrNR14:
            hwio_nr14 = value;
            break;
        case kAddrNR21:
            hwio_nr21 = value;
            break;
        case kAddrNR22:
            hwio_nr22 = value;
            break;
        case kAddrNR23:
            hwio_nr23 = value;
            break;
        case kAddrNR24:
            hwio_nr24 = value;
            break;
        case kAddrNR30:
            hwio_nr30 = value & 0x80;
            break;
        case kAddrNR31:
            hwio_nr31 = value;
            break;
        case kAddrNR32:
            hwio_nr32 = value & 0x60;
            break;
        case kAddrNR33:
            hwio_nr33 = value;
            break;
        case kAddrNR34:
            hwio_nr34 = value;
            break;
        case kAddrNR41:
            hwio_nr41 = value & 0x3f;
            break;
        case kAddrNR42:
            hwio_nr42 = value;
            break;
        case kAddrNR43:
            hwio_nr43 = value;
            break;
        case kAddrNR44:
            hwio_nr41 = value & 0xc0;
            break;
        case kAddrNR50:
            hwio_nr50 = value;
            break;
        case kAddrNR51:
            hwio_nr51 = value;
            break;
        case kAddrNR52:
            hwio_nr52 = value;
            break;
        case kAddrWave + 0x0:
        case kAddrWave + 0x1:
        case kAddrWave + 0x2:
        case kAddrWave + 0x3:
        case kAddrWave + 0x4:
        case kAddrWave + 0x5:
        case kAddrWave + 0x6:
        case kAddrWave + 0x7:
        case kAddrWave + 0x8:
        case kAddrWave + 0x9:
        case kAddrWave + 0xa:
        case kAddrWave + 0xb:
        case kAddrWave + 0xc:
        case kAddrWave + 0xd:
        case kAddrWave + 0xe:
        case kAddrWave + 0xf:
            hwio_wave[addr - kAddrWave] = value;
            break;
        case kAddrLCDC:
            hwio_lcdc = value;
            break;
        case kAddrSTAT:
            hwio_stat = (value & 0x7c) | (hwio_stat & 0x03);
            break;
        case kAddrSCY:
            hwio_scy = value;
            break;
        case kAddrSCX:
            hwio_scx = value;
            break;
        case kAddrLY:
            hwio_ly = 0;
            hwio_stat = (hwio_stat & 0xfc) | 0x2;
            check_lcdc_line_coincidence();
            break;
        case kAddrLYC:
            hwio_lyc = value;
            check_lcdc_line_coincidence();
            break;
        case kAddrDMA: {
            uint16_t src_addr = value << 8;
            const uint16_t kSizeDMABlock = 0xa0;
            if (src_addr < (kAddrCROM + kSizeCROM - kSizeDMABlock)) {
                // from cartridge
                // std::cout << "dma copy from rom " << std::hex << (uint16_t) src_addr << "\n";
                cartridge->dma_read(src_addr, kSizeDMABlock, oram.begin());
            } else if (src_addr >= kAddrIRAM && src_addr < (kAddrIRAM + kSizeIRAM - kSizeDMABlock)) {
                // from IRAM
                // std::cout << "dma copy from ram " << std::hex << (uint16_t) src_addr << "\n";
                src_addr -= kAddrIRAM;
                std::copy(
                    iram.begin() + src_addr,
                    iram.begin() + src_addr + kSizeDMABlock,
                    oram.begin());
            } else if (src_addr >= kAddrHRAM && src_addr < (kAddrHRAM + kSizeHRAM - kSizeDMABlock)) {
                // from HRAM
                //std::cout << "dma copy from hram " << std::hex << (uint16_t) src_addr << "\n";
                src_addr -= kAddrHRAM;
                std::copy(
                    hram.begin() + src_addr,
                    hram.begin() + src_addr + kSizeDMABlock,
                    oram.begin());
            } else {
                std::cerr << "dma error " << std::hex << (uint16_t) src_addr << "\n";
            }
            // std::cout << print_bytes(oram.begin(), oram.begin() + kSizeDMABlock).str().c_str();
            // std::cout << "\n";
            break;
        }
        case kAddrBGP:
            hwio_bgp = value;
            break;
        case kAddrOBP0:
            hwio_obp0 = value;
            break;
        case kAddrOBP1:
            hwio_obp1 = value;
            break;
        case kAddrWY:
            hwio_wy = value;
            break;
        case kAddrWX:
            hwio_wx = value;
            break;
        case kAddrUnloadBIOS:
            bios_loaded = (value) ? 0 : 1;
            break;
        default:
            //std::cout << "ign w @" << addr << " " << (uint16_t) value << "\n";
            break;
    }
}

void GBMMU::check_lcdc_line_coincidence() {
    if (hwio_ly == hwio_lyc) {
        hwio_stat |= 0x40;
        request_lcdc_interrupt(LCDC_INTERRUPT_COINCI);
    } else {
        hwio_stat &= ~0x40;
    }
}

uint8_t* GBMMU::get_oam_ram_head() {
    return oram.data();
}

uint16_t GBMMU::get_oam_ram_size() {
    return oram.size();
}

void dump_mmu_oper(const char* op, uint16_t offset, uint16_t value) {
    std::cout << std::hex;
    std::cout << op << " @" << offset << " ";
    std::cout << value << "\n";
    std::cout << std::dec;
}

