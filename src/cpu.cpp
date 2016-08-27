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

Z80::Z80() {
    init_instrunction_map();
    init_cb_instrunction_map();
}

void Z80::init_instrunction_map() {
    instruction_map.reserve(256);
    instruction_map[0x00] = &Z80::nop;
    instruction_map[0x01] = &Z80::ld_bc_nn;
    instruction_map[0x02] = &Z80::ld_pbc_a;
    instruction_map[0x03] = &Z80::inc_bc;
    instruction_map[0x04] = &Z80::inc_b;
    instruction_map[0x05] = &Z80::dec_b;
    instruction_map[0x06] = &Z80::ld_b_n;
    instruction_map[0x07] = &Z80::rlca;
    instruction_map[0x08] = &Z80::ld_pnn_sp;
    instruction_map[0x09] = &Z80::add_hl_bc;
    instruction_map[0x0A] = &Z80::ld_a_pbc;
    instruction_map[0x0B] = &Z80::dec_bc;
    instruction_map[0x0C] = &Z80::inc_c;
    instruction_map[0x0D] = &Z80::dec_c;
    instruction_map[0x0E] = &Z80::ld_c_n;
    instruction_map[0x0F] = &Z80::rrca;

    instruction_map[0x10] = &Z80::stop;
    instruction_map[0x11] = &Z80::ld_de_nn;
    instruction_map[0x12] = &Z80::ld_pde_a;
    instruction_map[0x13] = &Z80::inc_de;
    instruction_map[0x14] = &Z80::inc_d;
    instruction_map[0x15] = &Z80::dec_d;
    instruction_map[0x16] = &Z80::ld_d_n;
    instruction_map[0x17] = &Z80::rla;
    instruction_map[0x18] = &Z80::jr;
    instruction_map[0x19] = &Z80::add_hl_de;
    instruction_map[0x1A] = &Z80::ld_a_pde;
    instruction_map[0x1B] = &Z80::dec_de;
    instruction_map[0x1C] = &Z80::inc_e;
    instruction_map[0x1D] = &Z80::dec_e;
    instruction_map[0x1E] = &Z80::ld_e_n;
    instruction_map[0x1F] = &Z80::rra;

    instruction_map[0x20] = &Z80::jr_nz;
    instruction_map[0x21] = &Z80::ld_hl_nn;
    instruction_map[0x22] = &Z80::ldi_phl_a;
    instruction_map[0x23] = &Z80::inc_hl;
    instruction_map[0x24] = &Z80::inc_h;
    instruction_map[0x25] = &Z80::dec_h;
    instruction_map[0x26] = &Z80::ld_h_n;
    instruction_map[0x27] = &Z80::not_implemented_error; // DAA
    instruction_map[0x28] = &Z80::jr_z;
    instruction_map[0x29] = &Z80::add_hl_hl;
    instruction_map[0x2A] = &Z80::ldi_a_phl;
    instruction_map[0x2B] = &Z80::dec_hl;
    instruction_map[0x2C] = &Z80::inc_l;
    instruction_map[0x2D] = &Z80::dec_l;
    instruction_map[0x2E] = &Z80::ld_l_n;
    instruction_map[0x2F] = &Z80::cpl;

    instruction_map[0x30] = &Z80::jr_nc;
    instruction_map[0x31] = &Z80::ld_sp_nn;
    instruction_map[0x32] = &Z80::ldd_phl_a;
    instruction_map[0x33] = &Z80::inc_sp;
    instruction_map[0x34] = &Z80::inc_phl;
    instruction_map[0x35] = &Z80::dec_phl;
    instruction_map[0x36] = &Z80::ld_phl_n;
    instruction_map[0x37] = &Z80::scf;
    instruction_map[0x38] = &Z80::jr_c;
    instruction_map[0x39] = &Z80::add_hl_sp;
    instruction_map[0x3A] = &Z80::ldd_a_phl;
    instruction_map[0x3B] = &Z80::dec_sp;
    instruction_map[0x3C] = &Z80::inc_a;
    instruction_map[0x3D] = &Z80::dec_a;
    instruction_map[0x3E] = &Z80::ld_a_n;
    instruction_map[0x3F] = &Z80::ccf;

    instruction_map[0x40] = &Z80::ld_b_b;
    instruction_map[0x41] = &Z80::ld_b_c;
    instruction_map[0x42] = &Z80::ld_b_d;
    instruction_map[0x43] = &Z80::ld_b_e;
    instruction_map[0x44] = &Z80::ld_b_h;
    instruction_map[0x45] = &Z80::ld_b_l;
    instruction_map[0x46] = &Z80::ld_b_phl;
    instruction_map[0x47] = &Z80::ld_b_a;
    instruction_map[0x48] = &Z80::ld_c_b;
    instruction_map[0x49] = &Z80::ld_c_c;
    instruction_map[0x4A] = &Z80::ld_c_d;
    instruction_map[0x4B] = &Z80::ld_c_e;
    instruction_map[0x4C] = &Z80::ld_c_h;
    instruction_map[0x4D] = &Z80::ld_c_l;
    instruction_map[0x4E] = &Z80::ld_c_phl;
    instruction_map[0x4F] = &Z80::ld_c_a;

    instruction_map[0x50] = &Z80::ld_d_b;
    instruction_map[0x51] = &Z80::ld_d_c;
    instruction_map[0x52] = &Z80::ld_d_d;
    instruction_map[0x53] = &Z80::ld_d_e;
    instruction_map[0x54] = &Z80::ld_d_h;
    instruction_map[0x55] = &Z80::ld_d_l;
    instruction_map[0x56] = &Z80::ld_d_phl;
    instruction_map[0x57] = &Z80::ld_d_a;
    instruction_map[0x58] = &Z80::ld_e_b;
    instruction_map[0x59] = &Z80::ld_e_c;
    instruction_map[0x5A] = &Z80::ld_e_d;
    instruction_map[0x5B] = &Z80::ld_e_e;
    instruction_map[0x5C] = &Z80::ld_e_h;
    instruction_map[0x5D] = &Z80::ld_e_l;
    instruction_map[0x5E] = &Z80::ld_e_phl;
    instruction_map[0x5F] = &Z80::ld_e_a;

    instruction_map[0x60] = &Z80::ld_h_b;
    instruction_map[0x61] = &Z80::ld_h_c;
    instruction_map[0x62] = &Z80::ld_h_d;
    instruction_map[0x63] = &Z80::ld_h_e;
    instruction_map[0x64] = &Z80::ld_h_h;
    instruction_map[0x65] = &Z80::ld_h_l;
    instruction_map[0x66] = &Z80::ld_h_phl;
    instruction_map[0x67] = &Z80::ld_h_a;
    instruction_map[0x68] = &Z80::ld_l_b;
    instruction_map[0x69] = &Z80::ld_l_c;
    instruction_map[0x6A] = &Z80::ld_l_d;
    instruction_map[0x6B] = &Z80::ld_l_e;
    instruction_map[0x6C] = &Z80::ld_l_h;
    instruction_map[0x6D] = &Z80::ld_l_l;
    instruction_map[0x6E] = &Z80::ld_l_phl;
    instruction_map[0x6F] = &Z80::ld_l_a;

    instruction_map[0x70] = &Z80::ld_phl_b;
    instruction_map[0x71] = &Z80::ld_phl_c;
    instruction_map[0x72] = &Z80::ld_phl_d;
    instruction_map[0x73] = &Z80::ld_phl_e;
    instruction_map[0x74] = &Z80::ld_phl_h;
    instruction_map[0x75] = &Z80::ld_phl_l;
    instruction_map[0x76] = &Z80::halt;
    instruction_map[0x77] = &Z80::ld_phl_a;
    instruction_map[0x78] = &Z80::ld_a_b;
    instruction_map[0x79] = &Z80::ld_a_c;
    instruction_map[0x7A] = &Z80::ld_a_d;
    instruction_map[0x7B] = &Z80::ld_a_e;
    instruction_map[0x7C] = &Z80::ld_a_h;
    instruction_map[0x7D] = &Z80::ld_a_l;
    instruction_map[0x7E] = &Z80::ld_a_phl;
    instruction_map[0x7F] = &Z80::ld_a_a;

    instruction_map[0x80] = &Z80::add_a_b;
    instruction_map[0x81] = &Z80::add_a_c;
    instruction_map[0x82] = &Z80::add_a_d;
    instruction_map[0x83] = &Z80::add_a_e;
    instruction_map[0x84] = &Z80::add_a_h;
    instruction_map[0x85] = &Z80::add_a_l;
    instruction_map[0x86] = &Z80::add_a_phl;
    instruction_map[0x87] = &Z80::add_a_a;
    instruction_map[0x88] = &Z80::adc_a_b;
    instruction_map[0x89] = &Z80::adc_a_c;
    instruction_map[0x8A] = &Z80::adc_a_d;
    instruction_map[0x8B] = &Z80::adc_a_e;
    instruction_map[0x8C] = &Z80::adc_a_h;
    instruction_map[0x8D] = &Z80::adc_a_l;
    instruction_map[0x8E] = &Z80::adc_a_phl;
    instruction_map[0x8F] = &Z80::adc_a_a;

    instruction_map[0x90] = &Z80::sub_b;
    instruction_map[0x91] = &Z80::sub_c;
    instruction_map[0x92] = &Z80::sub_d;
    instruction_map[0x93] = &Z80::sub_e;
    instruction_map[0x94] = &Z80::sub_h;
    instruction_map[0x95] = &Z80::sub_l;
    instruction_map[0x96] = &Z80::sub_phl;
    instruction_map[0x97] = &Z80::sub_a;
    instruction_map[0x98] = &Z80::sbc_a_b;
    instruction_map[0x99] = &Z80::sbc_a_c;
    instruction_map[0x9A] = &Z80::sbc_a_d;
    instruction_map[0x9B] = &Z80::sbc_a_e;
    instruction_map[0x9C] = &Z80::sbc_a_h;
    instruction_map[0x9D] = &Z80::sbc_a_l;
    instruction_map[0x9E] = &Z80::sbc_a_phl;
    instruction_map[0x9F] = &Z80::sbc_a_a;

    instruction_map[0xA0] = &Z80::and_b;
    instruction_map[0xA1] = &Z80::and_c;
    instruction_map[0xA2] = &Z80::and_d;
    instruction_map[0xA3] = &Z80::and_e;
    instruction_map[0xA4] = &Z80::and_h;
    instruction_map[0xA5] = &Z80::and_l;
    instruction_map[0xA6] = &Z80::and_phl;
    instruction_map[0xA7] = &Z80::and_a;
    instruction_map[0xA8] = &Z80::xor_b;
    instruction_map[0xA9] = &Z80::xor_c;
    instruction_map[0xAA] = &Z80::xor_d;
    instruction_map[0xAB] = &Z80::xor_e;
    instruction_map[0xAC] = &Z80::xor_h;
    instruction_map[0xAD] = &Z80::xor_l;
    instruction_map[0xAE] = &Z80::xor_phl;
    instruction_map[0xAF] = &Z80::xor_a;

    instruction_map[0xB0] = &Z80::or_b;
    instruction_map[0xB1] = &Z80::or_c;
    instruction_map[0xB2] = &Z80::or_d;
    instruction_map[0xB3] = &Z80::or_e;
    instruction_map[0xB4] = &Z80::or_h;
    instruction_map[0xB5] = &Z80::or_l;
    instruction_map[0xB6] = &Z80::or_phl;
    instruction_map[0xB7] = &Z80::or_a;
    instruction_map[0xB8] = &Z80::cp_b;
    instruction_map[0xB9] = &Z80::cp_c;
    instruction_map[0xBA] = &Z80::cp_d;
    instruction_map[0xBB] = &Z80::cp_e;
    instruction_map[0xBC] = &Z80::cp_h;
    instruction_map[0xBD] = &Z80::cp_l;
    instruction_map[0xBE] = &Z80::cp_phl;
    instruction_map[0xBF] = &Z80::cp_a;

    instruction_map[0xC0] = &Z80::ret_nz;
    instruction_map[0xC1] = &Z80::pop_bc;
    instruction_map[0xC2] = &Z80::jp_nz;
    instruction_map[0xC3] = &Z80::jp;
    instruction_map[0xC4] = &Z80::call_nz;
    instruction_map[0xC5] = &Z80::push_bc;
    instruction_map[0xC6] = &Z80::add_a_n;
    instruction_map[0xC7] = &Z80::rst_00;
    instruction_map[0xC8] = &Z80::ret_z;
    instruction_map[0xC9] = &Z80::ret;
    instruction_map[0xCA] = &Z80::jp_z;
    instruction_map[0xCB] = &Z80::cb_branch;
    instruction_map[0xCC] = &Z80::call_z;
    instruction_map[0xCD] = &Z80::call;
    instruction_map[0xCE] = &Z80::adc_a_n;
    instruction_map[0xCF] = &Z80::rst_08;

    instruction_map[0xD0] = &Z80::ret_nc;
    instruction_map[0xD1] = &Z80::pop_de;
    instruction_map[0xD2] = &Z80::jp_nc;
    instruction_map[0xD3] = &Z80::not_supported_error;
    instruction_map[0xD4] = &Z80::call_nc;
    instruction_map[0xD5] = &Z80::push_de;
    instruction_map[0xD6] = &Z80::sub_n;
    instruction_map[0xD7] = &Z80::rst_10;
    instruction_map[0xD8] = &Z80::ret_c;
    instruction_map[0xD9] = &Z80::reti;
    instruction_map[0xDA] = &Z80::jp_c;
    instruction_map[0xDB] = &Z80::not_supported_error;
    instruction_map[0xDC] = &Z80::call_c;
    instruction_map[0xDD] = &Z80::not_supported_error;
    instruction_map[0xDE] = &Z80::sbc_a_n;
    instruction_map[0xDF] = &Z80::rst_18;

    instruction_map[0xE0] = &Z80::ldh_offn_a;
    instruction_map[0xE1] = &Z80::pop_hl;
    instruction_map[0xE2] = &Z80::ld_offc_a;
    instruction_map[0xE3] = &Z80::not_supported_error;
    instruction_map[0xE4] = &Z80::not_supported_error;
    instruction_map[0xE5] = &Z80::push_hl;
    instruction_map[0xE6] = &Z80::and_n;
    instruction_map[0xE7] = &Z80::rst_20;
    instruction_map[0xE8] = &Z80::add_sp_n;
    instruction_map[0xE9] = &Z80::jp_hl;
    instruction_map[0xEA] = &Z80::ld_pnn_a;
    instruction_map[0xEB] = &Z80::not_supported_error;
    instruction_map[0xEC] = &Z80::not_supported_error;
    instruction_map[0xED] = &Z80::not_supported_error;
    instruction_map[0xEE] = &Z80::xor_n;
    instruction_map[0xEF] = &Z80::rst_28;

    instruction_map[0xF0] = &Z80::ldh_a_offn;
    instruction_map[0xF1] = &Z80::pop_af;
    instruction_map[0xF2] = &Z80::ld_a_offc;
    instruction_map[0xF3] = &Z80::di;
    instruction_map[0xF4] = &Z80::not_supported_error;
    instruction_map[0xF5] = &Z80::push_af;
    instruction_map[0xF6] = &Z80::or_n;
    instruction_map[0xF7] = &Z80::rst_30;
    instruction_map[0xF8] = &Z80::ld_hl_spn;
    instruction_map[0xF9] = &Z80::ld_sp_hl;
    instruction_map[0xFA] = &Z80::ld_a_pnn;
    instruction_map[0xFB] = &Z80::ei;
    instruction_map[0xFC] = &Z80::not_supported_error;
    instruction_map[0xFD] = &Z80::not_supported_error;
    instruction_map[0xFE] = &Z80::cp_n;
    instruction_map[0xFF] = &Z80::rst_38;
}

void Z80::init_cb_instrunction_map() {
    cb_instruction_map.reserve(256);

    cb_instruction_map[0x00] = &Z80::rlc_b;
    cb_instruction_map[0x01] = &Z80::rlc_c;
    cb_instruction_map[0x02] = &Z80::rlc_d;
    cb_instruction_map[0x03] = &Z80::rlc_e;
    cb_instruction_map[0x04] = &Z80::rlc_h;
    cb_instruction_map[0x05] = &Z80::rlc_l;
    cb_instruction_map[0x06] = &Z80::rlc_phl;
    cb_instruction_map[0x07] = &Z80::rlc_a;
    cb_instruction_map[0x08] = &Z80::rrc_b;
    cb_instruction_map[0x09] = &Z80::rrc_c;
    cb_instruction_map[0x0A] = &Z80::rrc_d;
    cb_instruction_map[0x0B] = &Z80::rrc_e;
    cb_instruction_map[0x0C] = &Z80::rrc_h;
    cb_instruction_map[0x0D] = &Z80::rrc_l;
    cb_instruction_map[0x0E] = &Z80::rrc_phl;
    cb_instruction_map[0x0F] = &Z80::rrc_a;

    cb_instruction_map[0x10] = &Z80::rl_b;
    cb_instruction_map[0x11] = &Z80::rl_c;
    cb_instruction_map[0x12] = &Z80::rl_d;
    cb_instruction_map[0x13] = &Z80::rl_e;
    cb_instruction_map[0x14] = &Z80::rl_h;
    cb_instruction_map[0x15] = &Z80::rl_l;
    cb_instruction_map[0x16] = &Z80::rl_phl;
    cb_instruction_map[0x17] = &Z80::rl_a;
    cb_instruction_map[0x18] = &Z80::rr_b;
    cb_instruction_map[0x19] = &Z80::rr_c;
    cb_instruction_map[0x1A] = &Z80::rr_d;
    cb_instruction_map[0x1B] = &Z80::rr_e;
    cb_instruction_map[0x1C] = &Z80::rr_h;
    cb_instruction_map[0x1D] = &Z80::rr_l;
    cb_instruction_map[0x1E] = &Z80::rr_phl;
    cb_instruction_map[0x1F] = &Z80::rr_a;

    cb_instruction_map[0x20] = &Z80::sla_b;
    cb_instruction_map[0x21] = &Z80::sla_c;
    cb_instruction_map[0x22] = &Z80::sla_d;
    cb_instruction_map[0x23] = &Z80::sla_e;
    cb_instruction_map[0x24] = &Z80::sla_h;
    cb_instruction_map[0x25] = &Z80::sla_l;
    cb_instruction_map[0x26] = &Z80::sla_phl;
    cb_instruction_map[0x27] = &Z80::sla_a;
    cb_instruction_map[0x28] = &Z80::sra_b;
    cb_instruction_map[0x29] = &Z80::sra_c;
    cb_instruction_map[0x2A] = &Z80::sra_d;
    cb_instruction_map[0x2B] = &Z80::sra_e;
    cb_instruction_map[0x2C] = &Z80::sra_h;
    cb_instruction_map[0x2D] = &Z80::sra_l;
    cb_instruction_map[0x2E] = &Z80::sra_phl;
    cb_instruction_map[0x2F] = &Z80::sra_a;

    cb_instruction_map[0x30] = &Z80::swap_b;
    cb_instruction_map[0x31] = &Z80::swap_c;
    cb_instruction_map[0x32] = &Z80::swap_d;
    cb_instruction_map[0x33] = &Z80::swap_e;
    cb_instruction_map[0x34] = &Z80::swap_h;
    cb_instruction_map[0x35] = &Z80::swap_l;
    cb_instruction_map[0x36] = &Z80::swap_phl;
    cb_instruction_map[0x37] = &Z80::swap_a;

    cb_instruction_map[0x40] = &Z80::bit_0_b;
    cb_instruction_map[0x41] = &Z80::bit_0_c;
    cb_instruction_map[0x42] = &Z80::bit_0_d;
    cb_instruction_map[0x43] = &Z80::bit_0_e;
    cb_instruction_map[0x44] = &Z80::bit_0_h;
    cb_instruction_map[0x45] = &Z80::bit_0_l;
    cb_instruction_map[0x46] = &Z80::bit_0_phl;
    cb_instruction_map[0x47] = &Z80::bit_0_a;
    cb_instruction_map[0x48] = &Z80::bit_1_b;
    cb_instruction_map[0x49] = &Z80::bit_1_c;
    cb_instruction_map[0x4A] = &Z80::bit_1_d;
    cb_instruction_map[0x4B] = &Z80::bit_1_e;
    cb_instruction_map[0x4C] = &Z80::bit_1_h;
    cb_instruction_map[0x4D] = &Z80::bit_1_l;
    cb_instruction_map[0x4E] = &Z80::bit_1_phl;
    cb_instruction_map[0x4F] = &Z80::bit_1_a;

    cb_instruction_map[0x50] = &Z80::bit_2_b;
    cb_instruction_map[0x51] = &Z80::bit_2_c;
    cb_instruction_map[0x52] = &Z80::bit_2_d;
    cb_instruction_map[0x53] = &Z80::bit_2_e;
    cb_instruction_map[0x54] = &Z80::bit_2_h;
    cb_instruction_map[0x55] = &Z80::bit_2_l;
    cb_instruction_map[0x56] = &Z80::bit_2_phl;
    cb_instruction_map[0x57] = &Z80::bit_2_a;
    cb_instruction_map[0x58] = &Z80::bit_3_b;
    cb_instruction_map[0x59] = &Z80::bit_3_c;
    cb_instruction_map[0x5A] = &Z80::bit_3_d;
    cb_instruction_map[0x5B] = &Z80::bit_3_e;
    cb_instruction_map[0x5C] = &Z80::bit_3_h;
    cb_instruction_map[0x5D] = &Z80::bit_3_l;
    cb_instruction_map[0x5E] = &Z80::bit_3_phl;
    cb_instruction_map[0x5F] = &Z80::bit_3_a;

    cb_instruction_map[0x60] = &Z80::bit_4_b;
    cb_instruction_map[0x61] = &Z80::bit_4_c;
    cb_instruction_map[0x62] = &Z80::bit_4_d;
    cb_instruction_map[0x63] = &Z80::bit_4_e;
    cb_instruction_map[0x64] = &Z80::bit_4_h;
    cb_instruction_map[0x65] = &Z80::bit_4_l;
    cb_instruction_map[0x66] = &Z80::bit_4_phl;
    cb_instruction_map[0x67] = &Z80::bit_4_a;
    cb_instruction_map[0x68] = &Z80::bit_5_b;
    cb_instruction_map[0x69] = &Z80::bit_5_c;
    cb_instruction_map[0x6A] = &Z80::bit_5_d;
    cb_instruction_map[0x6B] = &Z80::bit_5_e;
    cb_instruction_map[0x6C] = &Z80::bit_5_h;
    cb_instruction_map[0x6D] = &Z80::bit_5_l;
    cb_instruction_map[0x6E] = &Z80::bit_5_phl;
    cb_instruction_map[0x6F] = &Z80::bit_5_a;

    cb_instruction_map[0x70] = &Z80::bit_6_b;
    cb_instruction_map[0x71] = &Z80::bit_6_c;
    cb_instruction_map[0x72] = &Z80::bit_6_d;
    cb_instruction_map[0x73] = &Z80::bit_6_e;
    cb_instruction_map[0x74] = &Z80::bit_6_h;
    cb_instruction_map[0x75] = &Z80::bit_6_l;
    cb_instruction_map[0x76] = &Z80::bit_6_phl;
    cb_instruction_map[0x77] = &Z80::bit_6_a;
    cb_instruction_map[0x78] = &Z80::bit_7_b;
    cb_instruction_map[0x79] = &Z80::bit_7_c;
    cb_instruction_map[0x7A] = &Z80::bit_7_d;
    cb_instruction_map[0x7B] = &Z80::bit_7_e;
    cb_instruction_map[0x7C] = &Z80::bit_7_h;
    cb_instruction_map[0x7D] = &Z80::bit_7_l;
    cb_instruction_map[0x7E] = &Z80::bit_7_phl;
    cb_instruction_map[0x7F] = &Z80::bit_7_a;

    cb_instruction_map[0x80] = &Z80::res_0_b;
    cb_instruction_map[0x81] = &Z80::res_0_c;
    cb_instruction_map[0x82] = &Z80::res_0_d;
    cb_instruction_map[0x83] = &Z80::res_0_e;
    cb_instruction_map[0x84] = &Z80::res_0_h;
    cb_instruction_map[0x85] = &Z80::res_0_l;
    cb_instruction_map[0x86] = &Z80::res_0_phl;
    cb_instruction_map[0x87] = &Z80::res_0_a;
    cb_instruction_map[0x88] = &Z80::res_1_b;
    cb_instruction_map[0x89] = &Z80::res_1_c;
    cb_instruction_map[0x8A] = &Z80::res_1_d;
    cb_instruction_map[0x8B] = &Z80::res_1_e;
    cb_instruction_map[0x8C] = &Z80::res_1_h;
    cb_instruction_map[0x8D] = &Z80::res_1_l;
    cb_instruction_map[0x8E] = &Z80::res_1_phl;
    cb_instruction_map[0x8F] = &Z80::res_1_a;

    cb_instruction_map[0x90] = &Z80::res_2_b;
    cb_instruction_map[0x91] = &Z80::res_2_c;
    cb_instruction_map[0x92] = &Z80::res_2_d;
    cb_instruction_map[0x93] = &Z80::res_2_e;
    cb_instruction_map[0x94] = &Z80::res_2_h;
    cb_instruction_map[0x95] = &Z80::res_2_l;
    cb_instruction_map[0x96] = &Z80::res_2_phl;
    cb_instruction_map[0x97] = &Z80::res_2_a;
    cb_instruction_map[0x98] = &Z80::res_3_b;
    cb_instruction_map[0x99] = &Z80::res_3_c;
    cb_instruction_map[0x9A] = &Z80::res_3_d;
    cb_instruction_map[0x9B] = &Z80::res_3_e;
    cb_instruction_map[0x9C] = &Z80::res_3_h;
    cb_instruction_map[0x9D] = &Z80::res_3_l;
    cb_instruction_map[0x9E] = &Z80::res_3_phl;
    cb_instruction_map[0x9F] = &Z80::res_3_a;

    cb_instruction_map[0xA0] = &Z80::res_4_b;
    cb_instruction_map[0xA1] = &Z80::res_4_c;
    cb_instruction_map[0xA2] = &Z80::res_4_d;
    cb_instruction_map[0xA3] = &Z80::res_4_e;
    cb_instruction_map[0xA4] = &Z80::res_4_h;
    cb_instruction_map[0xA5] = &Z80::res_4_l;
    cb_instruction_map[0xA6] = &Z80::res_4_phl;
    cb_instruction_map[0xA7] = &Z80::res_4_a;
    cb_instruction_map[0xA8] = &Z80::res_5_b;
    cb_instruction_map[0xA9] = &Z80::res_5_c;
    cb_instruction_map[0xAA] = &Z80::res_5_d;
    cb_instruction_map[0xAB] = &Z80::res_5_e;
    cb_instruction_map[0xAC] = &Z80::res_5_h;
    cb_instruction_map[0xAD] = &Z80::res_5_l;
    cb_instruction_map[0xAE] = &Z80::res_5_phl;
    cb_instruction_map[0xAF] = &Z80::res_5_a;

    cb_instruction_map[0xB0] = &Z80::res_6_b;
    cb_instruction_map[0xB1] = &Z80::res_6_c;
    cb_instruction_map[0xB2] = &Z80::res_6_d;
    cb_instruction_map[0xB3] = &Z80::res_6_e;
    cb_instruction_map[0xB4] = &Z80::res_6_h;
    cb_instruction_map[0xB5] = &Z80::res_6_l;
    cb_instruction_map[0xB6] = &Z80::res_6_phl;
    cb_instruction_map[0xB7] = &Z80::res_6_a;
    cb_instruction_map[0xB8] = &Z80::res_7_b;
    cb_instruction_map[0xB9] = &Z80::res_7_c;
    cb_instruction_map[0xBA] = &Z80::res_7_d;
    cb_instruction_map[0xBB] = &Z80::res_7_e;
    cb_instruction_map[0xBC] = &Z80::res_7_h;
    cb_instruction_map[0xBD] = &Z80::res_7_l;
    cb_instruction_map[0xBE] = &Z80::res_7_phl;
    cb_instruction_map[0xBF] = &Z80::res_7_a;

    cb_instruction_map[0xC0] = &Z80::set_0_b;
    cb_instruction_map[0xC1] = &Z80::set_0_c;
    cb_instruction_map[0xC2] = &Z80::set_0_d;
    cb_instruction_map[0xC3] = &Z80::set_0_e;
    cb_instruction_map[0xC4] = &Z80::set_0_h;
    cb_instruction_map[0xC5] = &Z80::set_0_l;
    cb_instruction_map[0xC6] = &Z80::set_0_phl;
    cb_instruction_map[0xC7] = &Z80::set_0_a;
    cb_instruction_map[0xC8] = &Z80::set_1_b;
    cb_instruction_map[0xC9] = &Z80::set_1_c;
    cb_instruction_map[0xCA] = &Z80::set_1_d;
    cb_instruction_map[0xCB] = &Z80::set_1_e;
    cb_instruction_map[0xCC] = &Z80::set_1_h;
    cb_instruction_map[0xCD] = &Z80::set_1_l;
    cb_instruction_map[0xCE] = &Z80::set_1_phl;
    cb_instruction_map[0xCF] = &Z80::set_1_a;

    cb_instruction_map[0xD0] = &Z80::set_2_b;
    cb_instruction_map[0xD1] = &Z80::set_2_c;
    cb_instruction_map[0xD2] = &Z80::set_2_d;
    cb_instruction_map[0xD3] = &Z80::set_2_e;
    cb_instruction_map[0xD4] = &Z80::set_2_h;
    cb_instruction_map[0xD5] = &Z80::set_2_l;
    cb_instruction_map[0xD6] = &Z80::set_2_phl;
    cb_instruction_map[0xD7] = &Z80::set_2_a;
    cb_instruction_map[0xD8] = &Z80::set_3_b;
    cb_instruction_map[0xD9] = &Z80::set_3_c;
    cb_instruction_map[0xDA] = &Z80::set_3_d;
    cb_instruction_map[0xDB] = &Z80::set_3_e;
    cb_instruction_map[0xDC] = &Z80::set_3_h;
    cb_instruction_map[0xDD] = &Z80::set_3_l;
    cb_instruction_map[0xDE] = &Z80::set_3_phl;
    cb_instruction_map[0xDF] = &Z80::set_3_a;

    cb_instruction_map[0xE0] = &Z80::set_4_b;
    cb_instruction_map[0xE1] = &Z80::set_4_c;
    cb_instruction_map[0xE2] = &Z80::set_4_d;
    cb_instruction_map[0xE3] = &Z80::set_4_e;
    cb_instruction_map[0xE4] = &Z80::set_4_h;
    cb_instruction_map[0xE5] = &Z80::set_4_l;
    cb_instruction_map[0xE6] = &Z80::set_4_phl;
    cb_instruction_map[0xE7] = &Z80::set_4_a;
    cb_instruction_map[0xE8] = &Z80::set_5_b;
    cb_instruction_map[0xE9] = &Z80::set_5_c;
    cb_instruction_map[0xEA] = &Z80::set_5_d;
    cb_instruction_map[0xEB] = &Z80::set_5_e;
    cb_instruction_map[0xEC] = &Z80::set_5_h;
    cb_instruction_map[0xED] = &Z80::set_5_l;
    cb_instruction_map[0xEE] = &Z80::set_5_phl;
    cb_instruction_map[0xEF] = &Z80::set_5_a;

    cb_instruction_map[0xF0] = &Z80::set_6_b;
    cb_instruction_map[0xF1] = &Z80::set_6_c;
    cb_instruction_map[0xF2] = &Z80::set_6_d;
    cb_instruction_map[0xF3] = &Z80::set_6_e;
    cb_instruction_map[0xF4] = &Z80::set_6_h;
    cb_instruction_map[0xF5] = &Z80::set_6_l;
    cb_instruction_map[0xF6] = &Z80::set_6_phl;
    cb_instruction_map[0xF7] = &Z80::set_6_a;
    cb_instruction_map[0xF8] = &Z80::set_7_b;
    cb_instruction_map[0xF9] = &Z80::set_7_c;
    cb_instruction_map[0xFA] = &Z80::set_7_d;
    cb_instruction_map[0xFB] = &Z80::set_7_e;
    cb_instruction_map[0xFC] = &Z80::set_7_h;
    cb_instruction_map[0xFD] = &Z80::set_7_l;
    cb_instruction_map[0xFE] = &Z80::set_7_phl;
    cb_instruction_map[0xFF] = &Z80::set_7_a;

}

void Z80::reset() {
    std::memset(this, 0, sizeof(Z80));
}

// Template Instruction

/**
 * LD r1,r2
 *
 * Put value r2 into r1
 */
void Z80::ld_r_r(uint8_t& dst, const uint8_t& src) {
    dst = src;
    clock += Clock(1);
}

/**
 * LD r,(HL)
 *
 * Put value from address HL into r
 */
void Z80::ld_r_prr(uint8_t& dst, const uint8_t& rh, const uint8_t& rl) {
    uint16_t addr = combine16(rh, rl);
    dst = mmu.read_byte(addr);

    clock += Clock(2);
}

/**
 * LD (HL),r
 *
 * Put value r into address from HL
 */
void Z80::ld_prr_r(const uint8_t& rh, const uint8_t& rl, const uint8_t& src) {
    uint16_t addr = combine16(rh, rl);
    mmu.write_byte(addr, src);

    clock += Clock(2);
}

void Z80::ld_r_n(uint8_t& r) {
    r = mmu.read_byte(reg.pc++);
    clock += Clock(2);
}

void Z80::ld_rr_nn(uint8_t& rh, uint8_t& rl) {
    rl = mmu.read_byte(reg.pc++);
    rh = mmu.read_byte(reg.pc++);
    clock += Clock(3);
}

/**
 * Push register pair onto the stack
 * Decrement stack pointer twice
*/
void Z80::push_rr(const uint8_t& rh, const uint8_t& rl) {
    mmu.write_byte(reg.sp--, rl);
    mmu.write_byte(reg.sp--, rh);
    clock += Clock(4);
}

/**
 * Pop two bytes from the stack into register pair
 * Increment stack pointer twice
*/
void Z80::pop_rr(uint8_t& rh, uint8_t& rl) {
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
void Z80::add_a_r(const uint8_t& r) {
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
void Z80::add_hl_rr(const uint8_t& rh, const uint8_t& rl) {
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
void Z80::adc_a_r(const uint8_t& r) {
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
void Z80::sub(const uint8_t& r) {
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
void Z80::sbc_a_r(const uint8_t& r) {
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
void Z80::and_r(const uint8_t& r) {
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
void Z80::or_r(const uint8_t& r) {
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
void Z80::xor_r(const uint8_t& r) {
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
void Z80::cp_r(const uint8_t& r) {
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
void Z80::inc_r(uint8_t& r) {
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
void Z80::dec_r(uint8_t& r) {
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
void Z80::swap_r(uint8_t& r) {
    r = static_cast<uint8_t>(((r << 4) & 0xf0) | ((r >> 4) & 0x0f));
    reg.f = check_z(r);
    clock += Clock(2);
}

/**
 * SWAP (HL)
 */
void Z80::swap_phl() {
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
void Z80::inc_rr(uint8_t& rh, uint8_t& rl) {
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
void Z80::dec_rr(uint8_t& rh, uint8_t& rl) {
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
void Z80::rst(const uint16_t addr) {
    mmu.write_word(reg.sp, reg.pc);
    reg.sp -= 2;
    reg.pc = addr;
    clock += Clock(8);
}

/**
 * No operation
 */
void Z80::nop() {
    clock += Clock(1);
}

void Z80::ld_a_pnn() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    reg.a = mmu.read_byte(addr);

    clock += Clock(4);
}

void Z80::ld_pnn_a() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    mmu.write_byte(addr, reg.a);

    clock += Clock(4);
}

void Z80::bit_i_r(const uint8_t index, const uint8_t& r) {
    uint8_t mask = static_cast<uint8_t>(1 << index);
    reg.f = (reg.f & kFlagC) | kFlagH | ((r & mask) ? kFlagZ : 0);
    clock += Clock(2);
}

void Z80::set_i_r(const uint8_t index, uint8_t& r) {
    uint8_t mask = static_cast<uint8_t>(1 << index);
    r |= mask;
    clock += Clock(2);
}

void Z80::res_i_r(const uint8_t index, uint8_t& r) {
    uint8_t mask = static_cast<uint8_t>(1 << index);
    r &= ~mask;
    clock += Clock(2);
}

void Z80::rlc_r(uint8_t& r) {
    bool has_carry = (r & 0x80) != 0;
    r = (r << 1) | (r >> 7);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void Z80::rrc_r(uint8_t& r) {
    bool has_carry = (r & 0x01) != 0;
    r = (r >> 1) | (r << 7);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void Z80::rl_r(uint8_t& r) {
    bool has_carry = (r & 0x80) != 0;
    r = (r << 1) | (reg.f & kFlagC ? 0x01 : 0x00);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void Z80::rr_r(uint8_t& r) {
    bool has_carry = (r & 0x01) != 0;
    r = (r >> 1) | (reg.f & kFlagC ? 0x80 : 0x00);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void Z80::sla_r(uint8_t& r) {
    bool has_carry = (r & 0x80) != 0;
    r = (r << 1);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void Z80::sra_r(uint8_t& r) {
    bool has_carry = (r & 0x01) != 0;
    r = (r & 0x80)| (r >> 1);
    reg.f = check_z(r) | (has_carry ? kFlagC : 0);
    clock += Clock(2);
}

void Z80::bit_i_phl(const uint8_t index) {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    bit_i_r(index, value);
    mmu.write_byte(addr, value);
}

void Z80::set_i_phl(const uint8_t index) {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    set_i_r(index, value);
    mmu.write_byte(addr, value);
}

void Z80::res_i_phl(const uint8_t index) {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    res_i_r(index, value);
    mmu.write_byte(addr, value);
}

void Z80::rlc_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    rlc_r(value);
    mmu.write_byte(addr, value);
}

void Z80::rrc_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    rrc_r(value);
    mmu.write_byte(addr, value);
}

void Z80::rl_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    rl_r(value);
    mmu.write_byte(addr, value);
}

void Z80::rr_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    rr_r(value);
    mmu.write_byte(addr, value);
}

void Z80::sla_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(2);

    sla_r(value);
    mmu.write_byte(addr, value);
}

void Z80::sra_phl() {
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
void Z80::halt() {
    clock += Clock(1);
}

/*
 * STOP
 *
 * Halt CPU & LCD display until button pressed
 */
void Z80::stop() {
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
void Z80::rlca() {
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
void Z80::rla() {
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
void Z80::rrca() {
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
void Z80::rra() {
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
void Z80::cpl() {
    reg.a = ~reg.a;
    reg.f |= kFlagN | kFlagH;

    clock += Clock(2);
}

void Z80::add_hl_sp() {
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
void Z80::scf() {
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
void Z80::ccf() {
    reg.f = (reg.f & kFlagZ) | (reg.f & kFlagC ? 0 : kFlagC);
    clock += Clock(1);
}

void Z80::add_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    add_a_r(value);
}

void Z80::add_a_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    add_a_r(value);
}

void Z80::add_sp_n() {
    int8_t offset = static_cast<uint8_t>(mmu.read_byte(reg.pc++));
    acc = reg.sp + offset;

    reg.f = check_h(acc) + check_c(acc);
    reg.sp = static_cast<uint16_t>(acc & 0xffff);
    clock += Clock(4);
}

void Z80::adc_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    adc_a_r(value);
}

void Z80::adc_a_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    adc_a_r(value);
}

void Z80::ld_sp_hl() {
    uint16_t hl = combine16(reg.h, reg.l);
    mmu.write_word(reg.sp, hl);
    clock += Clock(2);
}

void Z80::ld_hl_spn() {
    int8_t offset = static_cast<int8_t>(mmu.read_byte(reg.pc++));
    acc = reg.sp + offset;

    reg.f = check_h(acc) | check_c(acc);
    clock += Clock(3);
}

void Z80::ld_phl_n() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(reg.pc++);
    mmu.write_byte(addr, value);
    clock += Clock(3);
}

/**
 * Put value at address FF00 + register C into A
 */
void Z80::ld_a_offc() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + reg.c);
    reg.a = mmu.read_byte(addr);

    clock += Clock(2);
}

/**
 * Put A into FF00 + register C
 */
void Z80::ld_offc_a() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + reg.c);
    mmu.write_byte(addr, reg.a);

    clock += Clock(2);
}

void Z80::ld_sp_nn() {
    uint8_t p = mmu.read_byte(reg.pc++);
    uint8_t s = mmu.read_byte(reg.pc++);
    reg.sp = combine16(s, p);

    clock += Clock(3);
}

void Z80::ld_pnn_sp() {
    uint8_t addr_lsb = mmu.read_byte(reg.pc++);
    uint8_t addr_msb = mmu.read_byte(reg.pc++);

    uint16_t addr = combine16(addr_msb, addr_lsb);
    mmu.write_word(addr, reg.sp);

    clock += Clock(5);
}

void Z80::ldh_offn_a() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + mmu.read_byte(reg.pc++));
    mmu.write_byte(addr, reg.a);

    clock += Clock(3);
}

void Z80::ldh_a_offn() {
    uint16_t addr = static_cast<uint16_t>(0xff00 + mmu.read_byte(reg.pc++));
    reg.a = mmu.read_byte(addr);

    clock += Clock(3);
}

void Z80::ldi_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    mmu.write_byte(addr, reg.a);
    inc_rr(reg.h, reg.l);
}

void Z80::ldi_phl_a() {
    uint16_t addr = combine16(reg.h, reg.l);
    reg.a = mmu.read_byte(addr);
    inc_rr(reg.h, reg.l);
}

void Z80::ldd_phl_a() {
    uint16_t addr = combine16(reg.h, reg.l);
    mmu.write_byte(addr, reg.a);
    dec_rr(reg.h, reg.l);
}

void Z80::ldd_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    reg.a = mmu.read_byte(addr);
    dec_rr(reg.h, reg.l);
}

void Z80::inc_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);

    bool half_carry = (value & 0x0f) == 0x0f;
    value += 1;

    reg.f = check_z(value) | (reg.f & kFlagC);
    reg.f |= half_carry ? kFlagH : 0;

    mmu.write_byte(addr, value);
    clock += Clock(3);
}

void Z80::dec_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);

    bool half_carry = (value & 0x18) == 0x10;
    value -= 1;

    reg.f = check_z(value) | (reg.f & kFlagC) | kFlagN;
    reg.f |= half_carry ? kFlagH : 0;

    mmu.write_byte(addr, value);
    clock += Clock(3);
}

void Z80::sub_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    sub(value);
}

void Z80::sub_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    sub(value);
}

void Z80::sbc_a_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    sbc_a_r(value);
}

void Z80::sbc_a_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    sbc_a_r(value);
}

void Z80::and_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    and_r(value);
}

void Z80::and_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    and_r(value);
}

void Z80::or_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    or_r(value);
}

void Z80::or_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    or_r(value);
}

void Z80::xor_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    xor_r(value);
}

void Z80::xor_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);
    xor_r(value);
}

void Z80::cp_phl() {
    uint16_t addr = combine16(reg.h, reg.l);
    uint8_t value = mmu.read_byte(addr);
    clock += Clock(1);
    cp_r(value);
}

void Z80::cp_n() {
    uint8_t value = mmu.read_byte(reg.pc++);
    clock += Clock(1);

    cp_r(value);
}

void Z80::di() {
    ime = false;
    clock += Clock(1);
}

void Z80::ei() {
    ime = true;
    clock += Clock(1);
}

void Z80::call() {
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

void Z80::call_z() {
    if ((reg.f & kFlagZ) != 0) {
        call();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::call_nz() {
    if ((reg.f & kFlagZ) == 0) {
        call();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::call_c() {
    if ((reg.f & kFlagC) != 0) {
        call();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::call_nc() {
    if ((reg.f & kFlagC) == 0) {
        call();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::ret() {
    uint8_t pc_msb = mmu.read_byte(++reg.sp);
    uint8_t pc_lsb = mmu.read_byte(++reg.sp);
    reg.pc = combine16(pc_msb, pc_lsb);

    clock += Clock(2);
}

void Z80::reti() {
    ret();
    ime = true;
}

void Z80::ret_z() {
    if ((reg.f & kFlagZ) != 0) {
        ret();
    } else {
        clock += Clock(2);
    }
}

void Z80::ret_nz() {
    if ((reg.f & kFlagZ) == 0) {
        ret();
    } else {
        clock += Clock(2);
    }
}

void Z80::ret_c() {
    if ((reg.f & kFlagC) != 0) {
        ret();
    } else {
        clock += Clock(2);
    }
}

void Z80::ret_nc() {
    if ((reg.f & kFlagC) == 0) {
        ret();
    } else {
        clock += Clock(2);
    }
}

void Z80::jp() {
    uint8_t l = mmu.read_byte(reg.pc++);
    uint8_t h = mmu.read_byte(reg.pc++);
    reg.pc = combine16(h, l);

    clock += Clock(3);
}

void Z80::jp_z() {
    if ((reg.f & kFlagZ) != 0) {
        jp();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::jp_nz() {
    if ((reg.f & kFlagZ) == 0) {
        jp();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::jp_c() {
    if ((reg.f & kFlagC) != 0) {
        jp();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::jp_nc() {
    if ((reg.f & kFlagC) == 0) {
        jp();
    } else {
        reg.pc += 2;
        clock += Clock(3);
    }
}

void Z80::jp_hl() {
    reg.pc = combine16(reg.h, reg.l);
    clock += Clock(1);
}

void Z80::jr() {
    int8_t offset = static_cast<int8_t>(mmu.read_byte(reg.pc++));
    acc = static_cast<int32_t>(reg.pc) + offset;
    reg.pc = static_cast<uint8_t>(acc);
    clock += Clock(2);
}

void Z80::jr_z() {
    if ((reg.f & kFlagZ) != 0) {
        jr();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void Z80::jr_nz() {
    if ((reg.f & kFlagZ) == 0) {
        jr();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void Z80::jr_c() {
    if ((reg.f & kFlagC) != 0) {
        jr();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}

void Z80::jr_nc() {
    if ((reg.f & kFlagC) == 0) {
        jr();
    } else {
        reg.pc++;
        clock += Clock(2);
    }
}
