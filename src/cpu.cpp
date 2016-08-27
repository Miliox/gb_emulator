#include "cpu.hpp"

#include <cstring>

const uint16_t kFlagZ = 1 << 7;
const uint16_t kFlagN = 1 << 6;
const uint16_t kFlagH = 1 << 5;
const uint16_t kFlagC = 1 << 4;

inline uint16_t check_z(int32_t value) {
    return (value == 0) ? kFlagZ : 0;
}

inline uint16_t check_h(uint32_t value) {
    return (value & 0x10) == 0x10 ? kFlagH : 0;
}

inline uint16_t check_c(int32_t value) {
    return (value > 0xff || value < 0) ? kFlagC : 0;
}

inline uint16_t check_h2(uint32_t value) {
    return (value & 0x10) == 0x1000 ? kFlagH : 0;
}

inline uint16_t check_c2(int32_t value) {
    return (value > 0xffff || value < 0) ? kFlagC : 0;
}

inline uint16_t combine16(const uint8_t& msb, const uint8_t& lsb) {
    return static_cast<uint16_t>((msb << 8) + lsb);
}

inline void split16(const uint16_t& src, uint8_t& msb, uint8_t& lsb) {
    msb = static_cast<uint8_t>(src >> 8);
    lsb = static_cast<uint8_t>(src);
}

GBCPU::GBCPU(GBMMU& mmu) :
    mmu(mmu),
    instruction_map(256, &GBCPU::not_implemented_error),
    cb_instruction_map(256, &GBCPU::not_implemented_error) {

    fill_instrunction_map();
    fill_cb_instrunction_map();
}

void GBCPU::fill_instrunction_map() {
    instruction_map.reserve(256);

    instruction_map[0x00] = &GBCPU::nop;
    instruction_map[0x01] = &GBCPU::ld_bc_nn;
    instruction_map[0x02] = &GBCPU::ld_pbc_a;
    instruction_map[0x03] = &GBCPU::inc_bc;
    instruction_map[0x04] = &GBCPU::inc_b;
    instruction_map[0x05] = &GBCPU::dec_b;
    instruction_map[0x06] = &GBCPU::ld_b_n;
    instruction_map[0x07] = &GBCPU::rlca;
    instruction_map[0x08] = &GBCPU::ld_pnn_sp;
    instruction_map[0x09] = &GBCPU::add_hl_bc;
    instruction_map[0x0A] = &GBCPU::ld_a_pbc;
    instruction_map[0x0B] = &GBCPU::dec_bc;
    instruction_map[0x0C] = &GBCPU::inc_c;
    instruction_map[0x0D] = &GBCPU::dec_c;
    instruction_map[0x0E] = &GBCPU::ld_c_n;
    instruction_map[0x0F] = &GBCPU::rrca;

    instruction_map[0x10] = &GBCPU::stop;
    instruction_map[0x11] = &GBCPU::ld_de_nn;
    instruction_map[0x12] = &GBCPU::ld_pde_a;
    instruction_map[0x13] = &GBCPU::inc_de;
    instruction_map[0x14] = &GBCPU::inc_d;
    instruction_map[0x15] = &GBCPU::dec_d;
    instruction_map[0x16] = &GBCPU::ld_d_n;
    instruction_map[0x17] = &GBCPU::rla;
    instruction_map[0x18] = &GBCPU::jr;
    instruction_map[0x19] = &GBCPU::add_hl_de;
    instruction_map[0x1A] = &GBCPU::ld_a_pde;
    instruction_map[0x1B] = &GBCPU::dec_de;
    instruction_map[0x1C] = &GBCPU::inc_e;
    instruction_map[0x1D] = &GBCPU::dec_e;
    instruction_map[0x1E] = &GBCPU::ld_e_n;
    instruction_map[0x1F] = &GBCPU::rra;

    instruction_map[0x20] = &GBCPU::jr_nz;
    instruction_map[0x21] = &GBCPU::ld_hl_nn;
    instruction_map[0x22] = &GBCPU::ldi_phl_a;
    instruction_map[0x23] = &GBCPU::inc_hl;
    instruction_map[0x24] = &GBCPU::inc_h;
    instruction_map[0x25] = &GBCPU::dec_h;
    instruction_map[0x26] = &GBCPU::ld_h_n;
    instruction_map[0x27] = &GBCPU::not_implemented_error; // DAA
    instruction_map[0x28] = &GBCPU::jr_z;
    instruction_map[0x29] = &GBCPU::add_hl_hl;
    instruction_map[0x2A] = &GBCPU::ldi_a_phl;
    instruction_map[0x2B] = &GBCPU::dec_hl;
    instruction_map[0x2C] = &GBCPU::inc_l;
    instruction_map[0x2D] = &GBCPU::dec_l;
    instruction_map[0x2E] = &GBCPU::ld_l_n;
    instruction_map[0x2F] = &GBCPU::cpl;

    instruction_map[0x30] = &GBCPU::jr_nc;
    instruction_map[0x31] = &GBCPU::ld_sp_nn;
    instruction_map[0x32] = &GBCPU::ldd_phl_a;
    instruction_map[0x33] = &GBCPU::inc_sp;
    instruction_map[0x34] = &GBCPU::inc_phl;
    instruction_map[0x35] = &GBCPU::dec_phl;
    instruction_map[0x36] = &GBCPU::ld_phl_n;
    instruction_map[0x37] = &GBCPU::scf;
    instruction_map[0x38] = &GBCPU::jr_c;
    instruction_map[0x39] = &GBCPU::add_hl_sp;
    instruction_map[0x3A] = &GBCPU::ldd_a_phl;
    instruction_map[0x3B] = &GBCPU::dec_sp;
    instruction_map[0x3C] = &GBCPU::inc_a;
    instruction_map[0x3D] = &GBCPU::dec_a;
    instruction_map[0x3E] = &GBCPU::ld_a_n;
    instruction_map[0x3F] = &GBCPU::ccf;

    instruction_map[0x40] = &GBCPU::ld_b_b;
    instruction_map[0x41] = &GBCPU::ld_b_c;
    instruction_map[0x42] = &GBCPU::ld_b_d;
    instruction_map[0x43] = &GBCPU::ld_b_e;
    instruction_map[0x44] = &GBCPU::ld_b_h;
    instruction_map[0x45] = &GBCPU::ld_b_l;
    instruction_map[0x46] = &GBCPU::ld_b_phl;
    instruction_map[0x47] = &GBCPU::ld_b_a;
    instruction_map[0x48] = &GBCPU::ld_c_b;
    instruction_map[0x49] = &GBCPU::ld_c_c;
    instruction_map[0x4A] = &GBCPU::ld_c_d;
    instruction_map[0x4B] = &GBCPU::ld_c_e;
    instruction_map[0x4C] = &GBCPU::ld_c_h;
    instruction_map[0x4D] = &GBCPU::ld_c_l;
    instruction_map[0x4E] = &GBCPU::ld_c_phl;
    instruction_map[0x4F] = &GBCPU::ld_c_a;

    instruction_map[0x50] = &GBCPU::ld_d_b;
    instruction_map[0x51] = &GBCPU::ld_d_c;
    instruction_map[0x52] = &GBCPU::ld_d_d;
    instruction_map[0x53] = &GBCPU::ld_d_e;
    instruction_map[0x54] = &GBCPU::ld_d_h;
    instruction_map[0x55] = &GBCPU::ld_d_l;
    instruction_map[0x56] = &GBCPU::ld_d_phl;
    instruction_map[0x57] = &GBCPU::ld_d_a;
    instruction_map[0x58] = &GBCPU::ld_e_b;
    instruction_map[0x59] = &GBCPU::ld_e_c;
    instruction_map[0x5A] = &GBCPU::ld_e_d;
    instruction_map[0x5B] = &GBCPU::ld_e_e;
    instruction_map[0x5C] = &GBCPU::ld_e_h;
    instruction_map[0x5D] = &GBCPU::ld_e_l;
    instruction_map[0x5E] = &GBCPU::ld_e_phl;
    instruction_map[0x5F] = &GBCPU::ld_e_a;

    instruction_map[0x60] = &GBCPU::ld_h_b;
    instruction_map[0x61] = &GBCPU::ld_h_c;
    instruction_map[0x62] = &GBCPU::ld_h_d;
    instruction_map[0x63] = &GBCPU::ld_h_e;
    instruction_map[0x64] = &GBCPU::ld_h_h;
    instruction_map[0x65] = &GBCPU::ld_h_l;
    instruction_map[0x66] = &GBCPU::ld_h_phl;
    instruction_map[0x67] = &GBCPU::ld_h_a;
    instruction_map[0x68] = &GBCPU::ld_l_b;
    instruction_map[0x69] = &GBCPU::ld_l_c;
    instruction_map[0x6A] = &GBCPU::ld_l_d;
    instruction_map[0x6B] = &GBCPU::ld_l_e;
    instruction_map[0x6C] = &GBCPU::ld_l_h;
    instruction_map[0x6D] = &GBCPU::ld_l_l;
    instruction_map[0x6E] = &GBCPU::ld_l_phl;
    instruction_map[0x6F] = &GBCPU::ld_l_a;

    instruction_map[0x70] = &GBCPU::ld_phl_b;
    instruction_map[0x71] = &GBCPU::ld_phl_c;
    instruction_map[0x72] = &GBCPU::ld_phl_d;
    instruction_map[0x73] = &GBCPU::ld_phl_e;
    instruction_map[0x74] = &GBCPU::ld_phl_h;
    instruction_map[0x75] = &GBCPU::ld_phl_l;
    instruction_map[0x76] = &GBCPU::halt;
    instruction_map[0x77] = &GBCPU::ld_phl_a;
    instruction_map[0x78] = &GBCPU::ld_a_b;
    instruction_map[0x79] = &GBCPU::ld_a_c;
    instruction_map[0x7A] = &GBCPU::ld_a_d;
    instruction_map[0x7B] = &GBCPU::ld_a_e;
    instruction_map[0x7C] = &GBCPU::ld_a_h;
    instruction_map[0x7D] = &GBCPU::ld_a_l;
    instruction_map[0x7E] = &GBCPU::ld_a_phl;
    instruction_map[0x7F] = &GBCPU::ld_a_a;

    instruction_map[0x80] = &GBCPU::add_a_b;
    instruction_map[0x81] = &GBCPU::add_a_c;
    instruction_map[0x82] = &GBCPU::add_a_d;
    instruction_map[0x83] = &GBCPU::add_a_e;
    instruction_map[0x84] = &GBCPU::add_a_h;
    instruction_map[0x85] = &GBCPU::add_a_l;
    instruction_map[0x86] = &GBCPU::add_a_phl;
    instruction_map[0x87] = &GBCPU::add_a_a;
    instruction_map[0x88] = &GBCPU::adc_a_b;
    instruction_map[0x89] = &GBCPU::adc_a_c;
    instruction_map[0x8A] = &GBCPU::adc_a_d;
    instruction_map[0x8B] = &GBCPU::adc_a_e;
    instruction_map[0x8C] = &GBCPU::adc_a_h;
    instruction_map[0x8D] = &GBCPU::adc_a_l;
    instruction_map[0x8E] = &GBCPU::adc_a_phl;
    instruction_map[0x8F] = &GBCPU::adc_a_a;

    instruction_map[0x90] = &GBCPU::sub_b;
    instruction_map[0x91] = &GBCPU::sub_c;
    instruction_map[0x92] = &GBCPU::sub_d;
    instruction_map[0x93] = &GBCPU::sub_e;
    instruction_map[0x94] = &GBCPU::sub_h;
    instruction_map[0x95] = &GBCPU::sub_l;
    instruction_map[0x96] = &GBCPU::sub_phl;
    instruction_map[0x97] = &GBCPU::sub_a;
    instruction_map[0x98] = &GBCPU::sbc_a_b;
    instruction_map[0x99] = &GBCPU::sbc_a_c;
    instruction_map[0x9A] = &GBCPU::sbc_a_d;
    instruction_map[0x9B] = &GBCPU::sbc_a_e;
    instruction_map[0x9C] = &GBCPU::sbc_a_h;
    instruction_map[0x9D] = &GBCPU::sbc_a_l;
    instruction_map[0x9E] = &GBCPU::sbc_a_phl;
    instruction_map[0x9F] = &GBCPU::sbc_a_a;

    instruction_map[0xA0] = &GBCPU::and_b;
    instruction_map[0xA1] = &GBCPU::and_c;
    instruction_map[0xA2] = &GBCPU::and_d;
    instruction_map[0xA3] = &GBCPU::and_e;
    instruction_map[0xA4] = &GBCPU::and_h;
    instruction_map[0xA5] = &GBCPU::and_l;
    instruction_map[0xA6] = &GBCPU::and_phl;
    instruction_map[0xA7] = &GBCPU::and_a;
    instruction_map[0xA8] = &GBCPU::xor_b;
    instruction_map[0xA9] = &GBCPU::xor_c;
    instruction_map[0xAA] = &GBCPU::xor_d;
    instruction_map[0xAB] = &GBCPU::xor_e;
    instruction_map[0xAC] = &GBCPU::xor_h;
    instruction_map[0xAD] = &GBCPU::xor_l;
    instruction_map[0xAE] = &GBCPU::xor_phl;
    instruction_map[0xAF] = &GBCPU::xor_a;

    instruction_map[0xB0] = &GBCPU::or_b;
    instruction_map[0xB1] = &GBCPU::or_c;
    instruction_map[0xB2] = &GBCPU::or_d;
    instruction_map[0xB3] = &GBCPU::or_e;
    instruction_map[0xB4] = &GBCPU::or_h;
    instruction_map[0xB5] = &GBCPU::or_l;
    instruction_map[0xB6] = &GBCPU::or_phl;
    instruction_map[0xB7] = &GBCPU::or_a;
    instruction_map[0xB8] = &GBCPU::cp_b;
    instruction_map[0xB9] = &GBCPU::cp_c;
    instruction_map[0xBA] = &GBCPU::cp_d;
    instruction_map[0xBB] = &GBCPU::cp_e;
    instruction_map[0xBC] = &GBCPU::cp_h;
    instruction_map[0xBD] = &GBCPU::cp_l;
    instruction_map[0xBE] = &GBCPU::cp_phl;
    instruction_map[0xBF] = &GBCPU::cp_a;

    instruction_map[0xC0] = &GBCPU::ret_nz;
    instruction_map[0xC1] = &GBCPU::pop_bc;
    instruction_map[0xC2] = &GBCPU::jp_nz;
    instruction_map[0xC3] = &GBCPU::jp;
    instruction_map[0xC4] = &GBCPU::call_nz;
    instruction_map[0xC5] = &GBCPU::push_bc;
    instruction_map[0xC6] = &GBCPU::add_a_n;
    instruction_map[0xC7] = &GBCPU::rst_00;
    instruction_map[0xC8] = &GBCPU::ret_z;
    instruction_map[0xC9] = &GBCPU::ret;
    instruction_map[0xCA] = &GBCPU::jp_z;
    instruction_map[0xCB] = &GBCPU::cb_branch;
    instruction_map[0xCC] = &GBCPU::call_z;
    instruction_map[0xCD] = &GBCPU::call;
    instruction_map[0xCE] = &GBCPU::adc_a_n;
    instruction_map[0xCF] = &GBCPU::rst_08;

    instruction_map[0xD0] = &GBCPU::ret_nc;
    instruction_map[0xD1] = &GBCPU::pop_de;
    instruction_map[0xD2] = &GBCPU::jp_nc;
    instruction_map[0xD3] = &GBCPU::not_supported_error;
    instruction_map[0xD4] = &GBCPU::call_nc;
    instruction_map[0xD5] = &GBCPU::push_de;
    instruction_map[0xD6] = &GBCPU::sub_n;
    instruction_map[0xD7] = &GBCPU::rst_10;
    instruction_map[0xD8] = &GBCPU::ret_c;
    instruction_map[0xD9] = &GBCPU::reti;
    instruction_map[0xDA] = &GBCPU::jp_c;
    instruction_map[0xDB] = &GBCPU::not_supported_error;
    instruction_map[0xDC] = &GBCPU::call_c;
    instruction_map[0xDD] = &GBCPU::not_supported_error;
    instruction_map[0xDE] = &GBCPU::sbc_a_n;
    instruction_map[0xDF] = &GBCPU::rst_18;

    instruction_map[0xE0] = &GBCPU::ldh_offn_a;
    instruction_map[0xE1] = &GBCPU::pop_hl;
    instruction_map[0xE2] = &GBCPU::ld_offc_a;
    instruction_map[0xE3] = &GBCPU::not_supported_error;
    instruction_map[0xE4] = &GBCPU::not_supported_error;
    instruction_map[0xE5] = &GBCPU::push_hl;
    instruction_map[0xE6] = &GBCPU::and_n;
    instruction_map[0xE7] = &GBCPU::rst_20;
    instruction_map[0xE8] = &GBCPU::add_sp_n;
    instruction_map[0xE9] = &GBCPU::jp_hl;
    instruction_map[0xEA] = &GBCPU::ld_pnn_a;
    instruction_map[0xEB] = &GBCPU::not_supported_error;
    instruction_map[0xEC] = &GBCPU::not_supported_error;
    instruction_map[0xED] = &GBCPU::not_supported_error;
    instruction_map[0xEE] = &GBCPU::xor_n;
    instruction_map[0xEF] = &GBCPU::rst_28;

    instruction_map[0xF0] = &GBCPU::ldh_a_offn;
    instruction_map[0xF1] = &GBCPU::pop_af;
    instruction_map[0xF2] = &GBCPU::ld_a_offc;
    instruction_map[0xF3] = &GBCPU::di;
    instruction_map[0xF4] = &GBCPU::not_supported_error;
    instruction_map[0xF5] = &GBCPU::push_af;
    instruction_map[0xF6] = &GBCPU::or_n;
    instruction_map[0xF7] = &GBCPU::rst_30;
    instruction_map[0xF8] = &GBCPU::ld_hl_spn;
    instruction_map[0xF9] = &GBCPU::ld_sp_hl;
    instruction_map[0xFA] = &GBCPU::ld_a_pnn;
    instruction_map[0xFB] = &GBCPU::ei;
    instruction_map[0xFC] = &GBCPU::not_supported_error;
    instruction_map[0xFD] = &GBCPU::not_supported_error;
    instruction_map[0xFE] = &GBCPU::cp_n;
    instruction_map[0xFF] = &GBCPU::rst_38;
}

void GBCPU::fill_cb_instrunction_map() {
    cb_instruction_map.reserve(256);

    cb_instruction_map[0x00] = &GBCPU::rlc_b;
    cb_instruction_map[0x01] = &GBCPU::rlc_c;
    cb_instruction_map[0x02] = &GBCPU::rlc_d;
    cb_instruction_map[0x03] = &GBCPU::rlc_e;
    cb_instruction_map[0x04] = &GBCPU::rlc_h;
    cb_instruction_map[0x05] = &GBCPU::rlc_l;
    cb_instruction_map[0x06] = &GBCPU::rlc_phl;
    cb_instruction_map[0x07] = &GBCPU::rlc_a;
    cb_instruction_map[0x08] = &GBCPU::rrc_b;
    cb_instruction_map[0x09] = &GBCPU::rrc_c;
    cb_instruction_map[0x0A] = &GBCPU::rrc_d;
    cb_instruction_map[0x0B] = &GBCPU::rrc_e;
    cb_instruction_map[0x0C] = &GBCPU::rrc_h;
    cb_instruction_map[0x0D] = &GBCPU::rrc_l;
    cb_instruction_map[0x0E] = &GBCPU::rrc_phl;
    cb_instruction_map[0x0F] = &GBCPU::rrc_a;

    cb_instruction_map[0x10] = &GBCPU::rl_b;
    cb_instruction_map[0x11] = &GBCPU::rl_c;
    cb_instruction_map[0x12] = &GBCPU::rl_d;
    cb_instruction_map[0x13] = &GBCPU::rl_e;
    cb_instruction_map[0x14] = &GBCPU::rl_h;
    cb_instruction_map[0x15] = &GBCPU::rl_l;
    cb_instruction_map[0x16] = &GBCPU::rl_phl;
    cb_instruction_map[0x17] = &GBCPU::rl_a;
    cb_instruction_map[0x18] = &GBCPU::rr_b;
    cb_instruction_map[0x19] = &GBCPU::rr_c;
    cb_instruction_map[0x1A] = &GBCPU::rr_d;
    cb_instruction_map[0x1B] = &GBCPU::rr_e;
    cb_instruction_map[0x1C] = &GBCPU::rr_h;
    cb_instruction_map[0x1D] = &GBCPU::rr_l;
    cb_instruction_map[0x1E] = &GBCPU::rr_phl;
    cb_instruction_map[0x1F] = &GBCPU::rr_a;

    cb_instruction_map[0x20] = &GBCPU::sla_b;
    cb_instruction_map[0x21] = &GBCPU::sla_c;
    cb_instruction_map[0x22] = &GBCPU::sla_d;
    cb_instruction_map[0x23] = &GBCPU::sla_e;
    cb_instruction_map[0x24] = &GBCPU::sla_h;
    cb_instruction_map[0x25] = &GBCPU::sla_l;
    cb_instruction_map[0x26] = &GBCPU::sla_phl;
    cb_instruction_map[0x27] = &GBCPU::sla_a;
    cb_instruction_map[0x28] = &GBCPU::sra_b;
    cb_instruction_map[0x29] = &GBCPU::sra_c;
    cb_instruction_map[0x2A] = &GBCPU::sra_d;
    cb_instruction_map[0x2B] = &GBCPU::sra_e;
    cb_instruction_map[0x2C] = &GBCPU::sra_h;
    cb_instruction_map[0x2D] = &GBCPU::sra_l;
    cb_instruction_map[0x2E] = &GBCPU::sra_phl;
    cb_instruction_map[0x2F] = &GBCPU::sra_a;

    cb_instruction_map[0x30] = &GBCPU::swap_b;
    cb_instruction_map[0x31] = &GBCPU::swap_c;
    cb_instruction_map[0x32] = &GBCPU::swap_d;
    cb_instruction_map[0x33] = &GBCPU::swap_e;
    cb_instruction_map[0x34] = &GBCPU::swap_h;
    cb_instruction_map[0x35] = &GBCPU::swap_l;
    cb_instruction_map[0x36] = &GBCPU::swap_phl;
    cb_instruction_map[0x37] = &GBCPU::swap_a;

    cb_instruction_map[0x40] = &GBCPU::bit_0_b;
    cb_instruction_map[0x41] = &GBCPU::bit_0_c;
    cb_instruction_map[0x42] = &GBCPU::bit_0_d;
    cb_instruction_map[0x43] = &GBCPU::bit_0_e;
    cb_instruction_map[0x44] = &GBCPU::bit_0_h;
    cb_instruction_map[0x45] = &GBCPU::bit_0_l;
    cb_instruction_map[0x46] = &GBCPU::bit_0_phl;
    cb_instruction_map[0x47] = &GBCPU::bit_0_a;
    cb_instruction_map[0x48] = &GBCPU::bit_1_b;
    cb_instruction_map[0x49] = &GBCPU::bit_1_c;
    cb_instruction_map[0x4A] = &GBCPU::bit_1_d;
    cb_instruction_map[0x4B] = &GBCPU::bit_1_e;
    cb_instruction_map[0x4C] = &GBCPU::bit_1_h;
    cb_instruction_map[0x4D] = &GBCPU::bit_1_l;
    cb_instruction_map[0x4E] = &GBCPU::bit_1_phl;
    cb_instruction_map[0x4F] = &GBCPU::bit_1_a;

    cb_instruction_map[0x50] = &GBCPU::bit_2_b;
    cb_instruction_map[0x51] = &GBCPU::bit_2_c;
    cb_instruction_map[0x52] = &GBCPU::bit_2_d;
    cb_instruction_map[0x53] = &GBCPU::bit_2_e;
    cb_instruction_map[0x54] = &GBCPU::bit_2_h;
    cb_instruction_map[0x55] = &GBCPU::bit_2_l;
    cb_instruction_map[0x56] = &GBCPU::bit_2_phl;
    cb_instruction_map[0x57] = &GBCPU::bit_2_a;
    cb_instruction_map[0x58] = &GBCPU::bit_3_b;
    cb_instruction_map[0x59] = &GBCPU::bit_3_c;
    cb_instruction_map[0x5A] = &GBCPU::bit_3_d;
    cb_instruction_map[0x5B] = &GBCPU::bit_3_e;
    cb_instruction_map[0x5C] = &GBCPU::bit_3_h;
    cb_instruction_map[0x5D] = &GBCPU::bit_3_l;
    cb_instruction_map[0x5E] = &GBCPU::bit_3_phl;
    cb_instruction_map[0x5F] = &GBCPU::bit_3_a;

    cb_instruction_map[0x60] = &GBCPU::bit_4_b;
    cb_instruction_map[0x61] = &GBCPU::bit_4_c;
    cb_instruction_map[0x62] = &GBCPU::bit_4_d;
    cb_instruction_map[0x63] = &GBCPU::bit_4_e;
    cb_instruction_map[0x64] = &GBCPU::bit_4_h;
    cb_instruction_map[0x65] = &GBCPU::bit_4_l;
    cb_instruction_map[0x66] = &GBCPU::bit_4_phl;
    cb_instruction_map[0x67] = &GBCPU::bit_4_a;
    cb_instruction_map[0x68] = &GBCPU::bit_5_b;
    cb_instruction_map[0x69] = &GBCPU::bit_5_c;
    cb_instruction_map[0x6A] = &GBCPU::bit_5_d;
    cb_instruction_map[0x6B] = &GBCPU::bit_5_e;
    cb_instruction_map[0x6C] = &GBCPU::bit_5_h;
    cb_instruction_map[0x6D] = &GBCPU::bit_5_l;
    cb_instruction_map[0x6E] = &GBCPU::bit_5_phl;
    cb_instruction_map[0x6F] = &GBCPU::bit_5_a;

    cb_instruction_map[0x70] = &GBCPU::bit_6_b;
    cb_instruction_map[0x71] = &GBCPU::bit_6_c;
    cb_instruction_map[0x72] = &GBCPU::bit_6_d;
    cb_instruction_map[0x73] = &GBCPU::bit_6_e;
    cb_instruction_map[0x74] = &GBCPU::bit_6_h;
    cb_instruction_map[0x75] = &GBCPU::bit_6_l;
    cb_instruction_map[0x76] = &GBCPU::bit_6_phl;
    cb_instruction_map[0x77] = &GBCPU::bit_6_a;
    cb_instruction_map[0x78] = &GBCPU::bit_7_b;
    cb_instruction_map[0x79] = &GBCPU::bit_7_c;
    cb_instruction_map[0x7A] = &GBCPU::bit_7_d;
    cb_instruction_map[0x7B] = &GBCPU::bit_7_e;
    cb_instruction_map[0x7C] = &GBCPU::bit_7_h;
    cb_instruction_map[0x7D] = &GBCPU::bit_7_l;
    cb_instruction_map[0x7E] = &GBCPU::bit_7_phl;
    cb_instruction_map[0x7F] = &GBCPU::bit_7_a;

    cb_instruction_map[0x80] = &GBCPU::res_0_b;
    cb_instruction_map[0x81] = &GBCPU::res_0_c;
    cb_instruction_map[0x82] = &GBCPU::res_0_d;
    cb_instruction_map[0x83] = &GBCPU::res_0_e;
    cb_instruction_map[0x84] = &GBCPU::res_0_h;
    cb_instruction_map[0x85] = &GBCPU::res_0_l;
    cb_instruction_map[0x86] = &GBCPU::res_0_phl;
    cb_instruction_map[0x87] = &GBCPU::res_0_a;
    cb_instruction_map[0x88] = &GBCPU::res_1_b;
    cb_instruction_map[0x89] = &GBCPU::res_1_c;
    cb_instruction_map[0x8A] = &GBCPU::res_1_d;
    cb_instruction_map[0x8B] = &GBCPU::res_1_e;
    cb_instruction_map[0x8C] = &GBCPU::res_1_h;
    cb_instruction_map[0x8D] = &GBCPU::res_1_l;
    cb_instruction_map[0x8E] = &GBCPU::res_1_phl;
    cb_instruction_map[0x8F] = &GBCPU::res_1_a;

    cb_instruction_map[0x90] = &GBCPU::res_2_b;
    cb_instruction_map[0x91] = &GBCPU::res_2_c;
    cb_instruction_map[0x92] = &GBCPU::res_2_d;
    cb_instruction_map[0x93] = &GBCPU::res_2_e;
    cb_instruction_map[0x94] = &GBCPU::res_2_h;
    cb_instruction_map[0x95] = &GBCPU::res_2_l;
    cb_instruction_map[0x96] = &GBCPU::res_2_phl;
    cb_instruction_map[0x97] = &GBCPU::res_2_a;
    cb_instruction_map[0x98] = &GBCPU::res_3_b;
    cb_instruction_map[0x99] = &GBCPU::res_3_c;
    cb_instruction_map[0x9A] = &GBCPU::res_3_d;
    cb_instruction_map[0x9B] = &GBCPU::res_3_e;
    cb_instruction_map[0x9C] = &GBCPU::res_3_h;
    cb_instruction_map[0x9D] = &GBCPU::res_3_l;
    cb_instruction_map[0x9E] = &GBCPU::res_3_phl;
    cb_instruction_map[0x9F] = &GBCPU::res_3_a;

    cb_instruction_map[0xA0] = &GBCPU::res_4_b;
    cb_instruction_map[0xA1] = &GBCPU::res_4_c;
    cb_instruction_map[0xA2] = &GBCPU::res_4_d;
    cb_instruction_map[0xA3] = &GBCPU::res_4_e;
    cb_instruction_map[0xA4] = &GBCPU::res_4_h;
    cb_instruction_map[0xA5] = &GBCPU::res_4_l;
    cb_instruction_map[0xA6] = &GBCPU::res_4_phl;
    cb_instruction_map[0xA7] = &GBCPU::res_4_a;
    cb_instruction_map[0xA8] = &GBCPU::res_5_b;
    cb_instruction_map[0xA9] = &GBCPU::res_5_c;
    cb_instruction_map[0xAA] = &GBCPU::res_5_d;
    cb_instruction_map[0xAB] = &GBCPU::res_5_e;
    cb_instruction_map[0xAC] = &GBCPU::res_5_h;
    cb_instruction_map[0xAD] = &GBCPU::res_5_l;
    cb_instruction_map[0xAE] = &GBCPU::res_5_phl;
    cb_instruction_map[0xAF] = &GBCPU::res_5_a;

    cb_instruction_map[0xB0] = &GBCPU::res_6_b;
    cb_instruction_map[0xB1] = &GBCPU::res_6_c;
    cb_instruction_map[0xB2] = &GBCPU::res_6_d;
    cb_instruction_map[0xB3] = &GBCPU::res_6_e;
    cb_instruction_map[0xB4] = &GBCPU::res_6_h;
    cb_instruction_map[0xB5] = &GBCPU::res_6_l;
    cb_instruction_map[0xB6] = &GBCPU::res_6_phl;
    cb_instruction_map[0xB7] = &GBCPU::res_6_a;
    cb_instruction_map[0xB8] = &GBCPU::res_7_b;
    cb_instruction_map[0xB9] = &GBCPU::res_7_c;
    cb_instruction_map[0xBA] = &GBCPU::res_7_d;
    cb_instruction_map[0xBB] = &GBCPU::res_7_e;
    cb_instruction_map[0xBC] = &GBCPU::res_7_h;
    cb_instruction_map[0xBD] = &GBCPU::res_7_l;
    cb_instruction_map[0xBE] = &GBCPU::res_7_phl;
    cb_instruction_map[0xBF] = &GBCPU::res_7_a;

    cb_instruction_map[0xC0] = &GBCPU::set_0_b;
    cb_instruction_map[0xC1] = &GBCPU::set_0_c;
    cb_instruction_map[0xC2] = &GBCPU::set_0_d;
    cb_instruction_map[0xC3] = &GBCPU::set_0_e;
    cb_instruction_map[0xC4] = &GBCPU::set_0_h;
    cb_instruction_map[0xC5] = &GBCPU::set_0_l;
    cb_instruction_map[0xC6] = &GBCPU::set_0_phl;
    cb_instruction_map[0xC7] = &GBCPU::set_0_a;
    cb_instruction_map[0xC8] = &GBCPU::set_1_b;
    cb_instruction_map[0xC9] = &GBCPU::set_1_c;
    cb_instruction_map[0xCA] = &GBCPU::set_1_d;
    cb_instruction_map[0xCB] = &GBCPU::set_1_e;
    cb_instruction_map[0xCC] = &GBCPU::set_1_h;
    cb_instruction_map[0xCD] = &GBCPU::set_1_l;
    cb_instruction_map[0xCE] = &GBCPU::set_1_phl;
    cb_instruction_map[0xCF] = &GBCPU::set_1_a;

    cb_instruction_map[0xD0] = &GBCPU::set_2_b;
    cb_instruction_map[0xD1] = &GBCPU::set_2_c;
    cb_instruction_map[0xD2] = &GBCPU::set_2_d;
    cb_instruction_map[0xD3] = &GBCPU::set_2_e;
    cb_instruction_map[0xD4] = &GBCPU::set_2_h;
    cb_instruction_map[0xD5] = &GBCPU::set_2_l;
    cb_instruction_map[0xD6] = &GBCPU::set_2_phl;
    cb_instruction_map[0xD7] = &GBCPU::set_2_a;
    cb_instruction_map[0xD8] = &GBCPU::set_3_b;
    cb_instruction_map[0xD9] = &GBCPU::set_3_c;
    cb_instruction_map[0xDA] = &GBCPU::set_3_d;
    cb_instruction_map[0xDB] = &GBCPU::set_3_e;
    cb_instruction_map[0xDC] = &GBCPU::set_3_h;
    cb_instruction_map[0xDD] = &GBCPU::set_3_l;
    cb_instruction_map[0xDE] = &GBCPU::set_3_phl;
    cb_instruction_map[0xDF] = &GBCPU::set_3_a;

    cb_instruction_map[0xE0] = &GBCPU::set_4_b;
    cb_instruction_map[0xE1] = &GBCPU::set_4_c;
    cb_instruction_map[0xE2] = &GBCPU::set_4_d;
    cb_instruction_map[0xE3] = &GBCPU::set_4_e;
    cb_instruction_map[0xE4] = &GBCPU::set_4_h;
    cb_instruction_map[0xE5] = &GBCPU::set_4_l;
    cb_instruction_map[0xE6] = &GBCPU::set_4_phl;
    cb_instruction_map[0xE7] = &GBCPU::set_4_a;
    cb_instruction_map[0xE8] = &GBCPU::set_5_b;
    cb_instruction_map[0xE9] = &GBCPU::set_5_c;
    cb_instruction_map[0xEA] = &GBCPU::set_5_d;
    cb_instruction_map[0xEB] = &GBCPU::set_5_e;
    cb_instruction_map[0xEC] = &GBCPU::set_5_h;
    cb_instruction_map[0xED] = &GBCPU::set_5_l;
    cb_instruction_map[0xEE] = &GBCPU::set_5_phl;
    cb_instruction_map[0xEF] = &GBCPU::set_5_a;

    cb_instruction_map[0xF0] = &GBCPU::set_6_b;
    cb_instruction_map[0xF1] = &GBCPU::set_6_c;
    cb_instruction_map[0xF2] = &GBCPU::set_6_d;
    cb_instruction_map[0xF3] = &GBCPU::set_6_e;
    cb_instruction_map[0xF4] = &GBCPU::set_6_h;
    cb_instruction_map[0xF5] = &GBCPU::set_6_l;
    cb_instruction_map[0xF6] = &GBCPU::set_6_phl;
    cb_instruction_map[0xF7] = &GBCPU::set_6_a;
    cb_instruction_map[0xF8] = &GBCPU::set_7_b;
    cb_instruction_map[0xF9] = &GBCPU::set_7_c;
    cb_instruction_map[0xFA] = &GBCPU::set_7_d;
    cb_instruction_map[0xFB] = &GBCPU::set_7_e;
    cb_instruction_map[0xFC] = &GBCPU::set_7_h;
    cb_instruction_map[0xFD] = &GBCPU::set_7_l;
    cb_instruction_map[0xFE] = &GBCPU::set_7_phl;
    cb_instruction_map[0xFF] = &GBCPU::set_7_a;

}

void GBCPU::reset() {
    std::memset(this, 0, sizeof(GBCPU));
}

// Template Instruction

/**
 * LD r1,r2
 *
 * Put value r2 into r1
 */
void GBCPU::ld_r_r(uint8_t& dst, const uint8_t& src) {
    dst = src;
    clock += Clock(1);
}

/**
 * LD r,(HL)
 *
 * Put value from address HL into r
 */
void GBCPU::ld_r_prr(uint8_t& dst, const uint8_t& rh, const uint8_t& rl) {
    uint16_t addr = combine16(rh, rl);
    dst = mmu.read_byte(addr);

    clock += Clock(2);
}

/**
 * LD (HL),r
 *
 * Put value r into address from HL
 */
void GBCPU::ld_prr_r(const uint8_t& rh, const uint8_t& rl, const uint8_t& src) {
    uint16_t addr = combine16(rh, rl);
    mmu.write_byte(addr, src);

    clock += Clock(2);
}

void GBCPU::ld_r_n(uint8_t& r) {
    r = mmu.read_byte(reg.pc++);
    clock += Clock(2);
}

void GBCPU::ld_rr_nn(uint8_t& rh, uint8_t& rl) {
    rl = mmu.read_byte(reg.pc++);
    rh = mmu.read_byte(reg.pc++);
    clock += Clock(3);
}

/**
 * Push register pair onto the stack
 * Decrement stack pointer twice
*/
void GBCPU::push_rr(const uint8_t& rh, const uint8_t& rl) {
    mmu.write_byte(reg.sp--, rl);
    mmu.write_byte(reg.sp--, rh);
    clock += Clock(4);
}

/**
 * Pop two bytes from the stack into register pair
 * Increment stack pointer twice
*/
void GBCPU::pop_rr(uint8_t& rh, uint8_t& rl) {
    rh = mmu.read_byte(++reg.sp);
    rl = mmu.read_byte(++reg.sp);
    clock += Clock(3);
}

/**
 * ADD A,r
 *
 * Add r to A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Set if carry from bit 3
 * C - Set if carry from bit 7
 */
void GBCPU::add_a_r(const uint8_t& r) {
    acc = reg.a + r;
    reg.f = check_z(acc & 0xff) + check_h(acc) + check_c(acc);
    reg.a = static_cast<uint8_t>(acc);

    clock += Clock(1);
}

/**
 * ADD HL,rr
 *
 * ADD rr to HL
 * rr: BC, DE, HL, SP
 *
 * Flags Affected:
 * Z - Not affected
 * N - Reset
 * H - Set if carry from bit 11
 * C - Set if carry from bit 15
 */
void GBCPU::add_hl_rr(const uint8_t& rh, const uint8_t& rl) {
    acc = (reg.h << 8) + reg.l;
    acc += (rh << 8) + rl;

    reg.f = (reg.f & kFlagZ) + check_h2(acc) + check_c2(acc);
    split16(acc, reg.h, reg.l);

    clock += Clock(2);
}

/**
 * ADC A,r
 *
 * ADC (r + carry) to A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Set if carry from bit 3
 * C - Set if carry from bit 7
 */
void GBCPU::adc_a_r(const uint8_t& r) {
    acc = reg.a + r;
    acc += reg.f & kFlagC ? 1 : 0;

    reg.f = check_z(acc & 0xff) + check_h(acc) + check_c(acc);
    reg.a = static_cast<uint8_t>(acc);

    clock += Clock(1);
}

/**
 * SUB r
 *
 * Subtract r from A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Set
 * H - Set if borrow from bit 4
 * C - Set if no borrow
 */
void GBCPU::sub(const uint8_t& r) {
    acc = reg.a - r;

    reg.f = check_z(acc & 0xff) | kFlagN;
    reg.f |= (reg.a < r) ? kFlagC : 0;
    reg.f |= ((reg.a > 0xff && r > 0xff) || reg.a < r) ? kFlagH : 0;
    reg.a = static_cast<uint8_t>(acc);

    clock += Clock(1);
}

/**
 * SBC r
 *
 * Subtract r from A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Set
 * H - Set if borrow from bit 4
 * C - Set if no borrow
 */
void GBCPU::sbc_a_r(const uint8_t& r) {
    acc = reg.a - r;
    acc -= reg.f & kFlagC ? 1 : 0;

    reg.f = check_z(acc & 0xff) | kFlagN;
    reg.f |= (reg.a < r) ? kFlagC : 0;
    reg.f |= ((reg.a > 0xff && r > 0xff) || reg.a < r) ? kFlagH : 0;
    reg.a = static_cast<uint8_t>(acc);

    clock += Clock(1);
}

/**
 * AND r
 *
 * Logical AND r with A, result in A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Set
 * C - Reset
 */
void GBCPU::and_r(const uint8_t& r) {
    reg.a &= r;

    reg.f = check_z(reg.a) | kFlagH;

    clock += Clock(1);
}

/**
 * OR r
 *
 * Logical OR r with A, result in A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Reset
 */
void GBCPU::or_r(const uint8_t& r) {
    reg.a |= r;

    reg.f = check_z(reg.a);

    clock += Clock(1);
}

/**
 * XOR r
 *
 * Logical exclusive OR r with register A, result in A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Reset
 */
void GBCPU::xor_r(const uint8_t& r) {
    reg.a = static_cast<uint8_t>((!r & reg.a) | (r & !reg.a));
    reg.f = check_z(reg.a);

    clock += Clock(1);
}

/**
 * CP r
 *
 * Compare register r with A
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero (A == r) like sub but result throw away
 * N - Set
 * H - Set if no borrow from bit 4
 * C - Set for no borrow (Set if A < n)
 */
void GBCPU::cp_r(const uint8_t& r) {
    acc = reg.a - r;

    reg.f = check_z(acc & 0xff) | kFlagN;
    reg.f |= (reg.a < r) ? kFlagC : 0;
    reg.f |= ((reg.a > 0xff && r > 0xff) || reg.a < r) ? kFlagH : 0;

    clock += Clock(1);
}

/**
 * INC r
 *
 * Increment register r by one
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Set if carry from bit 3
 * C - Not affected
 */
void GBCPU::inc_r(uint8_t& r) {
    bool half_carry = (r & 0x0f) == 0x0f;
    r += 1;

    reg.f = check_z(r) | (reg.f & kFlagC);
    reg.f |= half_carry ? kFlagH : 0;

    clock += Clock(1);
}

/**
 * DEC r
 *
 * Decrement register r by one
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Set
 * H - Set if no borrow from bit 4
 * C - Not affected
 */
void GBCPU::dec_r(uint8_t& r) {
    bool half_carry = (r & 0x18) == 0x10;
    r -= 1;

    reg.f = check_z(r) | (reg.f & kFlagC) | kFlagN;
    reg.f |= half_carry ? kFlagH : 0;

    clock += Clock(1);
}

/**
 * SWAP r
 *
 * Swap upper & lower nibbles of r
 * r: A, B, C, D, E, H or L
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Reset
 */
void GBCPU::swap_r(uint8_t& r) {
    r = static_cast<uint8_t>(((r << 4) & 0xf0) | ((r >> 4) & 0x0f));
    reg.f = check_z(r);
    clock += Clock(2);
}

/**
 * SWAP (HL)
 */
void GBCPU::swap_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);
    swap_r(value);
    mmu.write_byte(addr, value);
}

/**
 * INC rr
 *
 * Increment rr register by one
 * rr: BC, DE, HL or SP
 */
void GBCPU::inc_rr(uint8_t& rh, uint8_t& rl) {
    uint16_t w = (rh << 8) | rl;
    w += 1;
    split16(w, rh, rl);

    clock += Clock(2);
}

/**
 * DEC rr
 *
 * Decrement rr register by one
 * rr: BC, DE, HL or SP
 */
void GBCPU::dec_rr(uint8_t& rh, uint8_t& rl) {
    uint16_t w = (rh << 8) | rl;
    w -= 1;
    split16(w, rh, rl);

    clock += Clock(2);
}

/**
  * RST n
  *
  * Push present address onto stack and jump to address 0x0000 + n
  */
void GBCPU::rst(const uint16_t addr) {
    mmu.write_word(reg.sp, reg.pc);
    reg.sp -= 2;
    reg.pc = addr;
    clock += Clock(8);
}

/**
 * No operation
 */
void GBCPU::nop() {
    clock += Clock(1);
}

void GBCPU::ld_a_pnn() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    reg.a = mmu.read_byte(addr);

    clock += Clock(4);
}

void GBCPU::ld_pnn_a() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    mmu.write_byte(addr, reg.a);

    clock += Clock(4);
}

void GBCPU::bit_i_r(const uint8_t index, const uint8_t& r) {
    uint8_t mask = static_cast<uint8_t>(1 << index);
    reg.f = (reg.f & kFlagC) | kFlagH | ((r & mask) ? kFlagZ : 0);
    clock += Clock(2);
}

void GBCPU::set_i_r(const uint8_t index, uint8_t& r) {
    uint8_t mask = static_cast<uint8_t>(1 << index);
    r |= mask;
    clock += Clock(2);
}

void GBCPU::res_i_r(const uint8_t index, uint8_t& r) {
    uint8_t mask = static_cast<uint8_t>(1 << index);
    r &= ~mask;
    clock += Clock(2);
}

void GBCPU::rlc_r(uint8_t& r) {
    bool has_carry = (r & 0x80) != 0;
    r = (r << 1) | (r >> 7);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void GBCPU::rrc_r(uint8_t& r) {
    bool has_carry = (r & 0x01) != 0;
    r = (r >> 1) | (r << 7);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void GBCPU::rl_r(uint8_t& r) {
    bool has_carry = (r & 0x80) != 0;
    r = (r << 1) | (reg.f & kFlagC ? 0x01 : 0x00);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void GBCPU::rr_r(uint8_t& r) {
    bool has_carry = (r & 0x01) != 0;
    r = (r >> 1) | (reg.f & kFlagC ? 0x80 : 0x00);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void GBCPU::sla_r(uint8_t& r) {
    bool has_carry = (r & 0x80) != 0;
    r = (r << 1);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void GBCPU::sra_r(uint8_t& r) {
    bool has_carry = (r & 0x01) != 0;
    r = (r & 0x80)| (r >> 1);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void GBCPU::bit_i_phl(const uint8_t index) {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    bit_i_r(index, value);
    mmu.write_byte(addr, value);
}

void GBCPU::set_i_phl(const uint8_t index) {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    set_i_r(index, value);
    mmu.write_byte(addr, value);
}

void GBCPU::res_i_phl(const uint8_t index) {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    res_i_r(index, value);
    mmu.write_byte(addr, value);
}

void GBCPU::rlc_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    rlc_r(value);
    mmu.write_byte(addr, value);
}

void GBCPU::rrc_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    rrc_r(value);
    mmu.write_byte(addr, value);
}

void GBCPU::rl_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    rl_r(value);
    mmu.write_byte(addr, value);
}

void GBCPU::rr_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    rr_r(value);
    mmu.write_byte(addr, value);
}

void GBCPU::sla_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    sla_r(value);
    mmu.write_byte(addr, value);
}

void GBCPU::sra_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    sra_r(value);
    mmu.write_byte(addr, value);
}

/**
 * HALT
 *
 * Power down CPU until an interrup occurs
 */
void GBCPU::halt() {
    clock += Clock(1);
}

/*
 * STOP
 *
 * Halt CPU & LCD display until button pressed
 */
void GBCPU::stop() {
    // TODO: Suspend until interruption
    clock += Clock(1);
}

/**
 * RLCA
 *
 * Rotate A left, Old bit 7 to carry flag
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Contains old bit 7
 */
void GBCPU::rlca() {
    bool has_carry = static_cast<bool>(reg.a & 0x80);

    reg.a <<= 1;
    reg.a += has_carry ? 1 : 0;
    reg.f = check_z(reg.a) | (has_carry ? kFlagC : 0);

    clock += Clock(1);
}

/**
 * RLA
 *
 * Rotate A left through carry
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Contains old bit 7
 */
void GBCPU::rla() {
    bool has_carry = static_cast<bool>(reg.a & 0x80);

    reg.a <<= 1;
    reg.a += (reg.f & kFlagC) ? 1 : 0;
    reg.f = check_z(reg.a) | (has_carry ? kFlagC : 0);

    clock += Clock(1);
}

/**
 * RLCA
 *
 * Rotate A right, Old bit 0 to carry flag
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Contains old bit 0
 */
void GBCPU::rrca() {
    bool has_carry = static_cast<bool>(reg.a & 0x01);

    reg.a >>= 1;
    reg.a += has_carry ? (1 << 7) : 0;
    reg.f = check_z(reg.a) | (has_carry ? kFlagC : 0);

    clock += Clock(1);
}

/**
 * RRA
 *
 * Rotate A right through carry
 *
 * Flags Affected:
 * Z - Set if result is zero
 * N - Reset
 * H - Reset
 * C - Contains old bit 0
 */
void GBCPU::rra() {
    bool has_carry = static_cast<bool>(reg.a & 0x01);

    reg.a <<= 1;
    reg.a += (reg.f & kFlagC) ? (1 << 7) : 0;
    reg.f = check_z(reg.a) | (has_carry ? kFlagC : 0);

    clock += Clock(1);
}

/*
 * CPL
 *
 * Complement A register
 *
 * Flags Affected:
 * Z - Not affected
 * N - Set
 * H - Set
 * C - Not affected
 */
void GBCPU::cpl() {
    reg.a = ~reg.a;
    reg.f |= kFlagN | kFlagH;

    clock += Clock(2);
}

void GBCPU::add_hl_sp() {
    uint8_t s = 0;
    uint8_t p = 0;
    split16(reg.sp, s, p);

    add_hl_rr(s, p);

    reg.sp = combine16(s, p);
}

/**
 * SCF
 * Set carry flag
 *
 * Flags Affected:
 * Z - Not affected
 * N - Reset
 * H - Reset
 * C - Set
 */
void GBCPU::scf() {
    reg.f = (reg.f & kFlagZ) | kFlagC;
    clock += Clock(1);
}

/**
 * CCF
 * Complement carry flag
 *
 * Flags Affected:
 * Z - Not affected
 * N - Reset
 * H - Reset
 * C - Complemented
 */
void GBCPU::ccf() {
    reg.f = (reg.f & kFlagZ) | (reg.f & kFlagC ? 0 : kFlagC);
    clock += Clock(1);
}

void GBCPU::add_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    add_a_r(value);
}

void GBCPU::add_a_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    add_a_r(value);
}

void GBCPU::add_sp_n() {
    int8_t offset = static_cast<uint8_t>(mmu.read_byte(reg.pc++));
    acc = reg.sp + offset;

    reg.f = check_h(acc) + check_c(acc);
    reg.sp = static_cast<uint16_t>(acc & 0xffff);
    clock += Clock(4);
}

void GBCPU::adc_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    adc_a_r(value);
}

void GBCPU::adc_a_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    adc_a_r(value);
}

void GBCPU::ld_sp_hl() {
    uint16_t hl = combine16(reg.h, reg.l);
    mmu.write_word(reg.sp, hl);
    clock += Clock(2);
}

void GBCPU::ld_hl_spn() {
    int8_t offset = static_cast<int8_t>(mmu.read_byte(reg.pc++));
    acc = reg.sp + offset;

    reg.f = check_h(acc) | check_c(acc);
    clock += Clock(3);
}

void GBCPU::ld_phl_n() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(reg.pc++);
    mmu.write_byte(addr, value);
    clock += Clock(3);
}

/**
 * Put value at address FF00 + register C into A
 */
void GBCPU::ld_a_offc() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + reg.c);
    reg.a = mmu.read_byte(addr);

    clock += Clock(2);
}

/**
 * Put A into FF00 + register C
 */
void GBCPU::ld_offc_a() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + reg.c);
    mmu.write_byte(addr, reg.a);

    clock += Clock(2);
}

void GBCPU::ld_sp_nn() {
    uint8_t p = mmu.read_byte(reg.pc++);
    uint8_t s = mmu.read_byte(reg.pc++);
    reg.sp = combine16(s, p);

    clock += Clock(3);
}

void GBCPU::ld_pnn_sp() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    mmu.write_word(addr, reg.sp);

    clock += Clock(5);
}

void GBCPU::ldh_offn_a() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + mmu.read_byte(reg.pc++));
    mmu.write_byte(addr, reg.a);

    clock += Clock(3);
}

void GBCPU::ldh_a_offn() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + mmu.read_byte(reg.pc++));
    reg.a = mmu.read_byte(addr);

    clock += Clock(3);
}

void GBCPU::ldi_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    mmu.write_byte(addr, reg.a);
    inc_rr(reg.h, reg.l);
}

void GBCPU::ldi_phl_a() {
    uint16_t addr = combine16(reg.h, reg.l);
    reg.a = mmu.read_byte(addr);
    inc_rr(reg.h, reg.l);
}

void GBCPU::ldd_phl_a() {
    uint16_t addr = combine16(reg.h, reg.l);
    mmu.write_byte(addr, reg.a);
    dec_rr(reg.h, reg.l);
}

void GBCPU::ldd_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    reg.a = mmu.read_byte(addr);
    dec_rr(reg.h, reg.l);
}

void GBCPU::inc_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);

    bool half_carry = (value & 0x0f) == 0x0f;
    value += 1;

    reg.f = check_z(value) | (reg.f & kFlagC);
    reg.f |= half_carry ? kFlagH : 0;

    mmu.write_byte(addr, value);
    clock += Clock(3);
}

void GBCPU::dec_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);

    bool half_carry = (value & 0x18) == 0x10;
    value -= 1;

    reg.f = check_z(value) | (reg.f & kFlagC) | kFlagN;
    reg.f |= half_carry ? kFlagH : 0;

    mmu.write_byte(addr, value);
    clock += Clock(3);
}

void GBCPU::sub_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    sub(value);
}

void GBCPU::sub_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    sub(value);
}

void GBCPU::sbc_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    sbc_a_r(value);
}

void GBCPU::sbc_a_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    sbc_a_r(value);
}

void GBCPU::and_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    and_r(value);
}

void GBCPU::and_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    and_r(value);
}

void GBCPU::or_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    or_r(value);
}

void GBCPU::or_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    or_r(value);
}

void GBCPU::xor_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    xor_r(value);
}

void GBCPU::xor_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    xor_r(value);
}

void GBCPU::cp_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    cp_r(value);
}

void GBCPU::cp_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);

    cp_r(value);
}

void GBCPU::di() {
    ime = false;
    clock += Clock(1);
}

void GBCPU::ei() {
    ime = true;
    clock += Clock(1);
}

void GBCPU::call() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint8_t pc_msb = 0;
    uint8_t pc_lsb = 0;
    split16(reg.pc, pc_msb, pc_lsb);

    mmu.write_byte(reg.sp--, pc_lsb);
    mmu.write_byte(reg.sp--, pc_msb);

    reg.pc = combine16(addr_msb, addr_lsb);
    clock += Clock(3);
}

void GBCPU::call_z() {
    if ((reg.f & kFlagZ) != 0) {
        call();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void GBCPU::call_nz() {
    if ((reg.f & kFlagZ) == 0) {
        call();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void GBCPU::call_c() {
    if ((reg.f & kFlagC) != 0) {
        call();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void GBCPU::call_nc() {
    if ((reg.f & kFlagC) == 0) {
        call();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void GBCPU::ret() {
    uint8_t pc_msb = mmu.read_byte(++reg.sp);
    uint8_t pc_lsb = mmu.read_byte(++reg.sp);
    reg.pc = combine16(pc_msb, pc_lsb);

    clock += Clock(2);
}

void GBCPU::reti() {
    ret();
    ime = true;
}

void GBCPU::ret_z() {
    if ((reg.f & kFlagZ) != 0) {
        ret();
    } else {
        clock += Clock(2);
    }
}

void GBCPU::ret_nz() {
    if ((reg.f & kFlagZ) == 0) {
        ret();
    } else {
        clock += Clock(2);
    }
}

void GBCPU::ret_c() {
    if ((reg.f & kFlagC) != 0) {
        ret();
    } else {
        clock += Clock(2);
    }
}

void GBCPU::ret_nc() {
    if ((reg.f & kFlagC) == 0) {
        ret();
    } else {
        clock += Clock(2);
    }
}

void GBCPU::jp() {
    uint8_t l = mmu.read_byte(reg.pc++);
    uint8_t h = mmu.read_byte(reg.pc++);
    reg.pc = combine16(h, l);

    clock += Clock(3);
}

void GBCPU::jp_z() {
    if ((reg.f & kFlagZ) != 0) {
        jp();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void GBCPU::jp_nz() {
    if ((reg.f & kFlagZ) == 0) {
        jp();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void GBCPU::jp_c() {
    if ((reg.f & kFlagC) != 0) {
        jp();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void GBCPU::jp_nc() {
    if ((reg.f & kFlagC) == 0) {
        jp();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void GBCPU::jp_hl() {
    reg.pc = combine16(reg.h, reg.l);
    clock += Clock(1);
}

void GBCPU::jr() {
    int8_t offset = static_cast<int8_t>(mmu.read_byte(reg.pc++));
    acc = static_cast<int32_t>(reg.pc) + offset;
    reg.pc = static_cast<uint8_t>(acc);
    clock += Clock(2);
}

void GBCPU::jr_z() {
    if ((reg.f & kFlagZ) != 0) {
        jr();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void GBCPU::jr_nz() {
    if ((reg.f & kFlagZ) == 0) {
        jr();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void GBCPU::jr_c() {
    if ((reg.f & kFlagC) != 0) {
        jr();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void GBCPU::jr_nc() {
    if ((reg.f & kFlagC) == 0) {
        jr();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}
