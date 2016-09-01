#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>

#include "clock.hpp"
#include "mmu.hpp"

class Registers {
public:
    struct {
        union {
            struct {
                uint8_t f; // flag
                uint8_t a; // accumulator
            };
            uint16_t af;
        };
    };

    struct {
        union {
            struct {
                uint8_t c;
                uint8_t b;
            };
            uint16_t bc;
        };
    };

    struct {
        union {
            struct {
                uint8_t e;
                uint8_t d;
            };
            uint16_t de;
        };
    };

    struct {
        union {
            struct {
                uint8_t l;
                uint8_t h;
            };
            uint16_t hl;
        };
    };

    uint16_t pc;  // program counter
    uint16_t sp;  // stack pointer

    Registers() : af(0), bc(0), de(0), hl(0), pc(0), sp(0) {}
};

class GBCPU {
private:
    int32_t acc;

    void fill_instrunction_map();
    void fill_cb_instrunction_map();
public:
    Registers reg;

    GBMMU& mmu;

    bool ime; // interrupt master enable flag

    std::vector<tick_t (GBCPU::*)()> instruction_map;
    std::vector<tick_t (GBCPU::*)()> cb_instruction_map;

    GBCPU(GBMMU& mmu);
    GBCPU(const GBCPU&) = delete;

    void reset();

    // Common Instruction Behavior
    tick_t ld_r_r   (uint8_t&  dst_reg,  uint8_t  src_reg);
    tick_t ld_r_prr (uint8_t&  dst_reg,  uint16_t src_addr);
    tick_t ld_prr_r (uint16_t  dst_addr, uint8_t  src_reg);
    tick_t ld_r_n   (uint8_t&  dst_reg);
    tick_t ld_rr_nn (uint16_t& dst_reg);

    tick_t push_rr  (uint16_t  src_reg);
    tick_t pop_rr   (uint16_t& dst_reg);

    tick_t add_a_r  (uint8_t  r);
    tick_t adc_a_r  (uint8_t  r);
    tick_t add_hl_rr(uint16_t r);
    tick_t adc_hl_rr(uint16_t r);
    tick_t sub      (uint8_t  r);
    tick_t sbc_a_r  (uint8_t  r);
    tick_t and_r    (uint8_t  r);
    tick_t or_r     (uint8_t  r);
    tick_t xor_r    (uint8_t  r);
    tick_t cp_r     (uint8_t  r);
    tick_t inc_r    (uint8_t& r);
    tick_t dec_r    (uint8_t& r);
    tick_t swap_r   (uint8_t& r);
    tick_t inc_rr   (uint16_t& r);
    tick_t dec_rr   (uint16_t& r);
    tick_t rst      (uint16_t addr);

    tick_t bit_i_r(const uint8_t index, const uint8_t& r);
    tick_t set_i_r(const uint8_t index, uint8_t& r);
    tick_t res_i_r(const uint8_t index, uint8_t& r);

    tick_t rlc_r(uint8_t& r);
    tick_t rrc_r(uint8_t& r);
    tick_t rl_r (uint8_t& r);
    tick_t rr_r (uint8_t& r);
    tick_t sla_r(uint8_t& r);
    tick_t sra_r(uint8_t& r);

    tick_t bit_i_phl(const uint8_t index);
    tick_t set_i_phl(const uint8_t index);
    tick_t res_i_phl(const uint8_t index);

    tick_t rlc_phl();
    tick_t rrc_phl();
    tick_t rl_phl();
    tick_t rr_phl();
    tick_t sla_phl();
    tick_t sra_phl();

    tick_t not_implemented_error() {
        throw std::runtime_error("Instruction Not Yet Implemented");
        return 0;
    }

    tick_t not_supported_error() {
        throw std::runtime_error("Instruction Not Supported");
        return 0;
    }

    tick_t cb_branch() {
        uint8_t op = mmu.read_byte(reg.pc++);
        return (*this.*cb_instruction_map.at(op))();
    }

    // Instruction Set
    tick_t nop();
    tick_t halt();
    tick_t stop();
    tick_t rlca();
    tick_t rla();
    tick_t rrca();
    tick_t rra();
    tick_t cpl();
    tick_t scf();
    tick_t ccf();
    tick_t di();
    tick_t ei();

    tick_t ld_a_a() { return ld_r_r(reg.a, reg.a); }
    tick_t ld_a_b() { return ld_r_r(reg.a, reg.b); }
    tick_t ld_a_c() { return ld_r_r(reg.a, reg.c); }
    tick_t ld_a_d() { return ld_r_r(reg.a, reg.d); }
    tick_t ld_a_e() { return ld_r_r(reg.a, reg.e); }
    tick_t ld_a_h() { return ld_r_r(reg.a, reg.h); }
    tick_t ld_a_l() { return ld_r_r(reg.a, reg.l); }
    tick_t ld_a_phl() { return ld_r_prr(reg.a, reg.hl); }

    tick_t ld_b_a() { return ld_r_r(reg.b, reg.a); }
    tick_t ld_b_b() { return ld_r_r(reg.b, reg.b); }
    tick_t ld_b_c() { return ld_r_r(reg.b, reg.c); }
    tick_t ld_b_d() { return ld_r_r(reg.b, reg.d); }
    tick_t ld_b_e() { return ld_r_r(reg.b, reg.e); }
    tick_t ld_b_h() { return ld_r_r(reg.b, reg.h); }
    tick_t ld_b_l() { return ld_r_r(reg.b, reg.l); }
    tick_t ld_b_phl() { return ld_r_prr(reg.b, reg.hl); }

    tick_t ld_c_a() { return ld_r_r(reg.c, reg.a); }
    tick_t ld_c_b() { return ld_r_r(reg.c, reg.b); }
    tick_t ld_c_c() { return ld_r_r(reg.c, reg.c); }
    tick_t ld_c_d() { return ld_r_r(reg.c, reg.d); }
    tick_t ld_c_e() { return ld_r_r(reg.c, reg.e); }
    tick_t ld_c_h() { return ld_r_r(reg.c, reg.h); }
    tick_t ld_c_l() { return ld_r_r(reg.c, reg.l); }
    tick_t ld_c_phl() { return ld_r_prr(reg.c, reg.hl); }

    tick_t ld_d_a() { return ld_r_r(reg.d, reg.a); }
    tick_t ld_d_b() { return ld_r_r(reg.d, reg.b); }
    tick_t ld_d_c() { return ld_r_r(reg.d, reg.c); }
    tick_t ld_d_d() { return ld_r_r(reg.d, reg.d); }
    tick_t ld_d_e() { return ld_r_r(reg.d, reg.e); }
    tick_t ld_d_h() { return ld_r_r(reg.d, reg.h); }
    tick_t ld_d_l() { return ld_r_r(reg.d, reg.l); }
    tick_t ld_d_phl() { return ld_r_prr(reg.d, reg.hl); }

    tick_t ld_e_a() { return ld_r_r(reg.e, reg.a); }
    tick_t ld_e_b() { return ld_r_r(reg.e, reg.b); }
    tick_t ld_e_c() { return ld_r_r(reg.e, reg.c); }
    tick_t ld_e_d() { return ld_r_r(reg.e, reg.d); }
    tick_t ld_e_e() { return ld_r_r(reg.e, reg.e); }
    tick_t ld_e_h() { return ld_r_r(reg.e, reg.h); }
    tick_t ld_e_l() { return ld_r_r(reg.e, reg.l); }
    tick_t ld_e_phl() { return ld_r_prr(reg.e, reg.hl); }

    tick_t ld_h_a() { return ld_r_r(reg.h, reg.a); }
    tick_t ld_h_b() { return ld_r_r(reg.h, reg.b); }
    tick_t ld_h_c() { return ld_r_r(reg.h, reg.c); }
    tick_t ld_h_d() { return ld_r_r(reg.h, reg.d); }
    tick_t ld_h_e() { return ld_r_r(reg.h, reg.e); }
    tick_t ld_h_h() { return ld_r_r(reg.h, reg.h); }
    tick_t ld_h_l() { return ld_r_r(reg.h, reg.l); }
    tick_t ld_h_phl() { return ld_r_prr(reg.h, reg.hl); }

    tick_t ld_l_a() { return ld_r_r(reg.l, reg.a); }
    tick_t ld_l_b() { return ld_r_r(reg.l, reg.b); }
    tick_t ld_l_c() { return ld_r_r(reg.l, reg.c); }
    tick_t ld_l_d() { return ld_r_r(reg.l, reg.d); }
    tick_t ld_l_e() { return ld_r_r(reg.l, reg.e); }
    tick_t ld_l_h() { return ld_r_r(reg.l, reg.h); }
    tick_t ld_l_l() { return ld_r_r(reg.l, reg.l); }
    tick_t ld_l_phl() { return ld_r_prr(reg.l, reg.hl); }

    tick_t ld_sp_hl();
    tick_t ld_hl_spn();

    tick_t ld_phl_a() { return ld_prr_r(reg.hl, reg.a); }
    tick_t ld_phl_b() { return ld_prr_r(reg.hl, reg.b); }
    tick_t ld_phl_c() { return ld_prr_r(reg.hl, reg.c); }
    tick_t ld_phl_d() { return ld_prr_r(reg.hl, reg.d); }
    tick_t ld_phl_e() { return ld_prr_r(reg.hl, reg.e); }
    tick_t ld_phl_h() { return ld_prr_r(reg.hl, reg.h); }
    tick_t ld_phl_l() { return ld_prr_r(reg.hl, reg.l); }
    tick_t ld_phl_n();

    tick_t ld_pbc_a() { return ld_prr_r(reg.bc, reg.a); }
    tick_t ld_pde_a() { return ld_prr_r(reg.de, reg.a); }

    tick_t ld_a_pbc() { return ld_r_prr(reg.a, reg.bc); }
    tick_t ld_a_pde() { return ld_r_prr(reg.a, reg.de); }
    tick_t ld_a_pnn();

    tick_t ld_pnn_a();

    tick_t ld_a_n() { return ld_r_n(reg.a); }
    tick_t ld_b_n() { return ld_r_n(reg.b); }
    tick_t ld_c_n() { return ld_r_n(reg.c); }
    tick_t ld_d_n() { return ld_r_n(reg.d); }
    tick_t ld_e_n() { return ld_r_n(reg.e); }
    tick_t ld_h_n() { return ld_r_n(reg.h); }
    tick_t ld_l_n() { return ld_r_n(reg.l); }

    tick_t ld_bc_nn() { return ld_rr_nn(reg.bc); }
    tick_t ld_de_nn() { return ld_rr_nn(reg.de); }
    tick_t ld_hl_nn() { return ld_rr_nn(reg.hl); }
    tick_t ld_sp_nn();

    tick_t ld_pnn_sp();

    tick_t ld_a_offc();
    tick_t ld_offc_a();

    tick_t ldh_offn_a();
    tick_t ldh_a_offn();

    tick_t ldi_a_phl();
    tick_t ldi_phl_a();

    tick_t ldd_a_phl();
    tick_t ldd_phl_a();

    tick_t add_a_a() { return add_a_r(reg.a); }
    tick_t add_a_b() { return add_a_r(reg.b); }
    tick_t add_a_c() { return add_a_r(reg.c); }
    tick_t add_a_d() { return add_a_r(reg.d); }
    tick_t add_a_e() { return add_a_r(reg.e); }
    tick_t add_a_h() { return add_a_r(reg.h); }
    tick_t add_a_l() { return add_a_r(reg.l); }
    tick_t add_a_n();
    tick_t add_a_phl();

    tick_t add_sp_n();

    tick_t adc_a_a() { return adc_a_r(reg.a); }
    tick_t adc_a_b() { return adc_a_r(reg.b); }
    tick_t adc_a_c() { return adc_a_r(reg.c); }
    tick_t adc_a_d() { return adc_a_r(reg.d); }
    tick_t adc_a_e() { return adc_a_r(reg.e); }
    tick_t adc_a_h() { return adc_a_r(reg.h); }
    tick_t adc_a_l() { return adc_a_r(reg.l); }
    tick_t adc_a_n();
    tick_t adc_a_phl();

    tick_t add_hl_bc() { return add_hl_rr(reg.bc); }
    tick_t add_hl_de() { return add_hl_rr(reg.de); }
    tick_t add_hl_hl() { return add_hl_rr(reg.hl); }
    tick_t add_hl_sp() { return add_hl_rr(reg.sp); }

    tick_t sub_a() { return sub(reg.a); }
    tick_t sub_b() { return sub(reg.b); }
    tick_t sub_c() { return sub(reg.c); }
    tick_t sub_d() { return sub(reg.d); }
    tick_t sub_e() { return sub(reg.e); }
    tick_t sub_h() { return sub(reg.h); }
    tick_t sub_l() { return sub(reg.l); }
    tick_t sub_n();
    tick_t sub_phl();

    tick_t sbc_a_a() { return sbc_a_r(reg.a); }
    tick_t sbc_a_b() { return sbc_a_r(reg.b); }
    tick_t sbc_a_c() { return sbc_a_r(reg.c); }
    tick_t sbc_a_d() { return sbc_a_r(reg.d); }
    tick_t sbc_a_e() { return sbc_a_r(reg.e); }
    tick_t sbc_a_h() { return sbc_a_r(reg.h); }
    tick_t sbc_a_l() { return sbc_a_r(reg.l); }
    tick_t sbc_a_n();
    tick_t sbc_a_phl();

    tick_t and_a() { return and_r(reg.a); }
    tick_t and_b() { return and_r(reg.b); }
    tick_t and_c() { return and_r(reg.c); }
    tick_t and_d() { return and_r(reg.d); }
    tick_t and_e() { return and_r(reg.e); }
    tick_t and_h() { return and_r(reg.h); }
    tick_t and_l() { return and_r(reg.l); }
    tick_t and_n();
    tick_t and_phl();

    tick_t or_a() { return or_r(reg.a); }
    tick_t or_b() { return or_r(reg.b); }
    tick_t or_c() { return or_r(reg.c); }
    tick_t or_d() { return or_r(reg.d); }
    tick_t or_e() { return or_r(reg.e); }
    tick_t or_h() { return or_r(reg.h); }
    tick_t or_l() { return or_r(reg.l); }
    tick_t or_n();
    tick_t or_phl();

    tick_t xor_a() { return xor_r(reg.a); }
    tick_t xor_b() { return xor_r(reg.b); }
    tick_t xor_c() { return xor_r(reg.c); }
    tick_t xor_d() { return xor_r(reg.d); }
    tick_t xor_e() { return xor_r(reg.e); }
    tick_t xor_h() { return xor_r(reg.h); }
    tick_t xor_l() { return xor_r(reg.l); }
    tick_t xor_n();
    tick_t xor_phl();

    tick_t cp_a() { return cp_r(reg.a); }
    tick_t cp_b() { return cp_r(reg.b); }
    tick_t cp_c() { return cp_r(reg.c); }
    tick_t cp_d() { return cp_r(reg.d); }
    tick_t cp_e() { return cp_r(reg.e); }
    tick_t cp_h() { return cp_r(reg.h); }
    tick_t cp_l() { return cp_r(reg.l); }
    tick_t cp_n();
    tick_t cp_phl();

    tick_t inc_a() { return inc_r(reg.a); };
    tick_t inc_b() { return inc_r(reg.b); };
    tick_t inc_c() { return inc_r(reg.c); };
    tick_t inc_d() { return inc_r(reg.d); };
    tick_t inc_e() { return inc_r(reg.e); };
    tick_t inc_h() { return inc_r(reg.h); };
    tick_t inc_l() { return inc_r(reg.l); };

    tick_t dec_a() { return dec_r(reg.a); }
    tick_t dec_b() { return dec_r(reg.b); }
    tick_t dec_c() { return dec_r(reg.c); }
    tick_t dec_d() { return dec_r(reg.d); }
    tick_t dec_e() { return dec_r(reg.e); }
    tick_t dec_h() { return dec_r(reg.h); }
    tick_t dec_l() { return dec_r(reg.l); }

    tick_t swap_a() { return swap_r(reg.a); }
    tick_t swap_b() { return swap_r(reg.b); }
    tick_t swap_c() { return swap_r(reg.c); }
    tick_t swap_d() { return swap_r(reg.d); }
    tick_t swap_e() { return swap_r(reg.e); }
    tick_t swap_h() { return swap_r(reg.h); }
    tick_t swap_l() { return swap_r(reg.l); }
    tick_t swap_phl();

    tick_t inc_bc() { return inc_rr(reg.bc); }
    tick_t inc_de() { return inc_rr(reg.de); }
    tick_t inc_hl() { return inc_rr(reg.hl); }
    tick_t inc_sp() { return inc_rr(reg.sp); }
    tick_t inc_phl();

    tick_t dec_bc() { return dec_rr(reg.bc); }
    tick_t dec_de() { return dec_rr(reg.de); }
    tick_t dec_hl() { return dec_rr(reg.hl); }
    tick_t dec_sp() { return dec_rr(reg.sp); }
    tick_t dec_phl();

    tick_t push_af() { return push_rr(reg.af); }
    tick_t push_bc() { return push_rr(reg.bc); }
    tick_t push_de() { return push_rr(reg.de); }
    tick_t push_hl() { return push_rr(reg.hl); }

    tick_t pop_af() { return pop_rr(reg.af); }
    tick_t pop_bc() { return pop_rr(reg.bc); }
    tick_t pop_de() { return pop_rr(reg.de); }
    tick_t pop_hl() { return pop_rr(reg.hl); }

    tick_t rst_00() { return rst(0x00); };
    tick_t rst_08() { return rst(0x08); };
    tick_t rst_10() { return rst(0x10); };
    tick_t rst_18() { return rst(0x18); };
    tick_t rst_20() { return rst(0x20); };
    tick_t rst_28() { return rst(0x28); };
    tick_t rst_30() { return rst(0x30); };
    tick_t rst_38() { return rst(0x38); };

    // interruption handlers
    tick_t rst_40() { return rst(0x40); };
    tick_t rst_48() { return rst(0x48); };
    tick_t rst_50() { return rst(0x50); };
    tick_t rst_58() { return rst(0x58); };
    tick_t rst_60() { return rst(0x60); };

    tick_t call();
    tick_t call_z();
    tick_t call_nz();
    tick_t call_c();
    tick_t call_nc();

    tick_t ret();
    tick_t reti();
    tick_t ret_z();
    tick_t ret_nz();
    tick_t ret_c();
    tick_t ret_nc();

    tick_t jp();
    tick_t jp_z();
    tick_t jp_nz();
    tick_t jp_c();
    tick_t jp_nc();
    tick_t jp_hl();

    tick_t jr();
    tick_t jr_z();
    tick_t jr_nz();
    tick_t jr_c();
    tick_t jr_nc();

    // CB Instruction Set

    tick_t rlc_a() { return rlc_r(reg.a); }
    tick_t rlc_b() { return rlc_r(reg.b); }
    tick_t rlc_c() { return rlc_r(reg.c); }
    tick_t rlc_d() { return rlc_r(reg.d); }
    tick_t rlc_e() { return rlc_r(reg.e); }
    tick_t rlc_h() { return rlc_r(reg.h); }
    tick_t rlc_l() { return rlc_r(reg.l); }

    tick_t rrc_a() { return rrc_r(reg.a); }
    tick_t rrc_b() { return rrc_r(reg.b); }
    tick_t rrc_c() { return rrc_r(reg.c); }
    tick_t rrc_d() { return rrc_r(reg.d); }
    tick_t rrc_e() { return rrc_r(reg.e); }
    tick_t rrc_h() { return rrc_r(reg.h); }
    tick_t rrc_l() { return rrc_r(reg.l); }

    tick_t rl_a() { return rl_r(reg.a); }
    tick_t rl_b() { return rl_r(reg.b); }
    tick_t rl_c() { return rl_r(reg.c); }
    tick_t rl_d() { return rl_r(reg.d); }
    tick_t rl_e() { return rl_r(reg.e); }
    tick_t rl_h() { return rl_r(reg.h); }
    tick_t rl_l() { return rl_r(reg.l); }

    tick_t rr_a() { return rr_r(reg.a); }
    tick_t rr_b() { return rr_r(reg.b); }
    tick_t rr_c() { return rr_r(reg.c); }
    tick_t rr_d() { return rr_r(reg.d); }
    tick_t rr_e() { return rr_r(reg.e); }
    tick_t rr_h() { return rr_r(reg.h); }
    tick_t rr_l() { return rr_r(reg.l); }

    tick_t sla_a() { return sla_r(reg.a); }
    tick_t sla_b() { return sla_r(reg.b); }
    tick_t sla_c() { return sla_r(reg.c); }
    tick_t sla_d() { return sla_r(reg.d); }
    tick_t sla_e() { return sla_r(reg.e); }
    tick_t sla_h() { return sla_r(reg.h); }
    tick_t sla_l() { return sla_r(reg.l); }

    tick_t sra_a() { return sra_r(reg.a); }
    tick_t sra_b() { return sra_r(reg.b); }
    tick_t sra_c() { return sra_r(reg.c); }
    tick_t sra_d() { return sra_r(reg.d); }
    tick_t sra_e() { return sra_r(reg.e); }
    tick_t sra_h() { return sra_r(reg.h); }
    tick_t sra_l() { return sra_r(reg.l); }

    tick_t bit_0_a() { return bit_i_r(0, reg.a); }
    tick_t bit_1_a() { return bit_i_r(1, reg.a); }
    tick_t bit_2_a() { return bit_i_r(2, reg.a); }
    tick_t bit_3_a() { return bit_i_r(3, reg.a); }
    tick_t bit_4_a() { return bit_i_r(4, reg.a); }
    tick_t bit_5_a() { return bit_i_r(5, reg.a); }
    tick_t bit_6_a() { return bit_i_r(6, reg.a); }
    tick_t bit_7_a() { return bit_i_r(7, reg.a); }

    tick_t bit_0_b() { return bit_i_r(0, reg.b); }
    tick_t bit_1_b() { return bit_i_r(1, reg.b); }
    tick_t bit_2_b() { return bit_i_r(2, reg.b); }
    tick_t bit_3_b() { return bit_i_r(3, reg.b); }
    tick_t bit_4_b() { return bit_i_r(4, reg.b); }
    tick_t bit_5_b() { return bit_i_r(5, reg.b); }
    tick_t bit_6_b() { return bit_i_r(6, reg.b); }
    tick_t bit_7_b() { return bit_i_r(7, reg.b); }

    tick_t bit_0_c() { return bit_i_r(0, reg.c); }
    tick_t bit_1_c() { return bit_i_r(1, reg.c); }
    tick_t bit_2_c() { return bit_i_r(2, reg.c); }
    tick_t bit_3_c() { return bit_i_r(3, reg.c); }
    tick_t bit_4_c() { return bit_i_r(4, reg.c); }
    tick_t bit_5_c() { return bit_i_r(5, reg.c); }
    tick_t bit_6_c() { return bit_i_r(6, reg.c); }
    tick_t bit_7_c() { return bit_i_r(7, reg.c); }

    tick_t bit_0_d() { return bit_i_r(0, reg.d); }
    tick_t bit_1_d() { return bit_i_r(1, reg.d); }
    tick_t bit_2_d() { return bit_i_r(2, reg.d); }
    tick_t bit_3_d() { return bit_i_r(3, reg.d); }
    tick_t bit_4_d() { return bit_i_r(4, reg.d); }
    tick_t bit_5_d() { return bit_i_r(5, reg.d); }
    tick_t bit_6_d() { return bit_i_r(6, reg.d); }
    tick_t bit_7_d() { return bit_i_r(7, reg.d); }

    tick_t bit_0_e() { return bit_i_r(0, reg.e); }
    tick_t bit_1_e() { return bit_i_r(1, reg.e); }
    tick_t bit_2_e() { return bit_i_r(2, reg.e); }
    tick_t bit_3_e() { return bit_i_r(3, reg.e); }
    tick_t bit_4_e() { return bit_i_r(4, reg.e); }
    tick_t bit_5_e() { return bit_i_r(5, reg.e); }
    tick_t bit_6_e() { return bit_i_r(6, reg.e); }
    tick_t bit_7_e() { return bit_i_r(7, reg.e); }

    tick_t bit_0_h() { return bit_i_r(0, reg.h); }
    tick_t bit_1_h() { return bit_i_r(1, reg.h); }
    tick_t bit_2_h() { return bit_i_r(2, reg.h); }
    tick_t bit_3_h() { return bit_i_r(3, reg.h); }
    tick_t bit_4_h() { return bit_i_r(4, reg.h); }
    tick_t bit_5_h() { return bit_i_r(5, reg.h); }
    tick_t bit_6_h() { return bit_i_r(6, reg.h); }
    tick_t bit_7_h() { return bit_i_r(7, reg.h); }

    tick_t bit_0_l() { return bit_i_r(0, reg.l); }
    tick_t bit_1_l() { return bit_i_r(1, reg.l); }
    tick_t bit_2_l() { return bit_i_r(2, reg.l); }
    tick_t bit_3_l() { return bit_i_r(3, reg.l); }
    tick_t bit_4_l() { return bit_i_r(4, reg.l); }
    tick_t bit_5_l() { return bit_i_r(5, reg.l); }
    tick_t bit_6_l() { return bit_i_r(6, reg.l); }
    tick_t bit_7_l() { return bit_i_r(7, reg.l); }

    tick_t bit_0_phl() { return bit_i_phl(0); }
    tick_t bit_1_phl() { return bit_i_phl(1); }
    tick_t bit_2_phl() { return bit_i_phl(2); }
    tick_t bit_3_phl() { return bit_i_phl(3); }
    tick_t bit_4_phl() { return bit_i_phl(4); }
    tick_t bit_5_phl() { return bit_i_phl(5); }
    tick_t bit_6_phl() { return bit_i_phl(6); }
    tick_t bit_7_phl() { return bit_i_phl(7); }


    tick_t set_0_a() { return set_i_r(0, reg.a); }
    tick_t set_1_a() { return set_i_r(1, reg.a); }
    tick_t set_2_a() { return set_i_r(2, reg.a); }
    tick_t set_3_a() { return set_i_r(3, reg.a); }
    tick_t set_4_a() { return set_i_r(4, reg.a); }
    tick_t set_5_a() { return set_i_r(5, reg.a); }
    tick_t set_6_a() { return set_i_r(6, reg.a); }
    tick_t set_7_a() { return set_i_r(7, reg.a); }

    tick_t set_0_b() { return set_i_r(0, reg.b); }
    tick_t set_1_b() { return set_i_r(1, reg.b); }
    tick_t set_2_b() { return set_i_r(2, reg.b); }
    tick_t set_3_b() { return set_i_r(3, reg.b); }
    tick_t set_4_b() { return set_i_r(4, reg.b); }
    tick_t set_5_b() { return set_i_r(5, reg.b); }
    tick_t set_6_b() { return set_i_r(6, reg.b); }
    tick_t set_7_b() { return set_i_r(7, reg.b); }

    tick_t set_0_c() { return set_i_r(0, reg.c); }
    tick_t set_1_c() { return set_i_r(1, reg.c); }
    tick_t set_2_c() { return set_i_r(2, reg.c); }
    tick_t set_3_c() { return set_i_r(3, reg.c); }
    tick_t set_4_c() { return set_i_r(4, reg.c); }
    tick_t set_5_c() { return set_i_r(5, reg.c); }
    tick_t set_6_c() { return set_i_r(6, reg.c); }
    tick_t set_7_c() { return set_i_r(7, reg.c); }

    tick_t set_0_d() { return set_i_r(0, reg.d); }
    tick_t set_1_d() { return set_i_r(1, reg.d); }
    tick_t set_2_d() { return set_i_r(2, reg.d); }
    tick_t set_3_d() { return set_i_r(3, reg.d); }
    tick_t set_4_d() { return set_i_r(4, reg.d); }
    tick_t set_5_d() { return set_i_r(5, reg.d); }
    tick_t set_6_d() { return set_i_r(6, reg.d); }
    tick_t set_7_d() { return set_i_r(7, reg.d); }

    tick_t set_0_e() { return set_i_r(0, reg.e); }
    tick_t set_1_e() { return set_i_r(1, reg.e); }
    tick_t set_2_e() { return set_i_r(2, reg.e); }
    tick_t set_3_e() { return set_i_r(3, reg.e); }
    tick_t set_4_e() { return set_i_r(4, reg.e); }
    tick_t set_5_e() { return set_i_r(5, reg.e); }
    tick_t set_6_e() { return set_i_r(6, reg.e); }
    tick_t set_7_e() { return set_i_r(7, reg.e); }

    tick_t set_0_h() { return set_i_r(0, reg.h); }
    tick_t set_1_h() { return set_i_r(1, reg.h); }
    tick_t set_2_h() { return set_i_r(2, reg.h); }
    tick_t set_3_h() { return set_i_r(3, reg.h); }
    tick_t set_4_h() { return set_i_r(4, reg.h); }
    tick_t set_5_h() { return set_i_r(5, reg.h); }
    tick_t set_6_h() { return set_i_r(6, reg.h); }
    tick_t set_7_h() { return set_i_r(7, reg.h); }

    tick_t set_0_l() { return set_i_r(0, reg.l); }
    tick_t set_1_l() { return set_i_r(1, reg.l); }
    tick_t set_2_l() { return set_i_r(2, reg.l); }
    tick_t set_3_l() { return set_i_r(3, reg.l); }
    tick_t set_4_l() { return set_i_r(4, reg.l); }
    tick_t set_5_l() { return set_i_r(5, reg.l); }
    tick_t set_6_l() { return set_i_r(6, reg.l); }
    tick_t set_7_l() { return set_i_r(7, reg.l); }

    tick_t set_0_phl() { return set_i_phl(0); }
    tick_t set_1_phl() { return set_i_phl(1); }
    tick_t set_2_phl() { return set_i_phl(2); }
    tick_t set_3_phl() { return set_i_phl(3); }
    tick_t set_4_phl() { return set_i_phl(4); }
    tick_t set_5_phl() { return set_i_phl(5); }
    tick_t set_6_phl() { return set_i_phl(6); }
    tick_t set_7_phl() { return set_i_phl(7); }

    tick_t res_0_a() { return res_i_r(0, reg.a); }
    tick_t res_1_a() { return res_i_r(1, reg.a); }
    tick_t res_2_a() { return res_i_r(2, reg.a); }
    tick_t res_3_a() { return res_i_r(3, reg.a); }
    tick_t res_4_a() { return res_i_r(4, reg.a); }
    tick_t res_5_a() { return res_i_r(5, reg.a); }
    tick_t res_6_a() { return res_i_r(6, reg.a); }
    tick_t res_7_a() { return res_i_r(7, reg.a); }

    tick_t res_0_b() { return res_i_r(0, reg.b); }
    tick_t res_1_b() { return res_i_r(1, reg.b); }
    tick_t res_2_b() { return res_i_r(2, reg.b); }
    tick_t res_3_b() { return res_i_r(3, reg.b); }
    tick_t res_4_b() { return res_i_r(4, reg.b); }
    tick_t res_5_b() { return res_i_r(5, reg.b); }
    tick_t res_6_b() { return res_i_r(6, reg.b); }
    tick_t res_7_b() { return res_i_r(7, reg.b); }

    tick_t res_0_c() { return res_i_r(0, reg.c); }
    tick_t res_1_c() { return res_i_r(1, reg.c); }
    tick_t res_2_c() { return res_i_r(2, reg.c); }
    tick_t res_3_c() { return res_i_r(3, reg.c); }
    tick_t res_4_c() { return res_i_r(4, reg.c); }
    tick_t res_5_c() { return res_i_r(5, reg.c); }
    tick_t res_6_c() { return res_i_r(6, reg.c); }
    tick_t res_7_c() { return res_i_r(7, reg.c); }

    tick_t res_0_d() { return res_i_r(0, reg.d); }
    tick_t res_1_d() { return res_i_r(1, reg.d); }
    tick_t res_2_d() { return res_i_r(2, reg.d); }
    tick_t res_3_d() { return res_i_r(3, reg.d); }
    tick_t res_4_d() { return res_i_r(4, reg.d); }
    tick_t res_5_d() { return res_i_r(5, reg.d); }
    tick_t res_6_d() { return res_i_r(6, reg.d); }
    tick_t res_7_d() { return res_i_r(7, reg.d); }

    tick_t res_0_e() { return res_i_r(0, reg.e); }
    tick_t res_1_e() { return res_i_r(1, reg.e); }
    tick_t res_2_e() { return res_i_r(2, reg.e); }
    tick_t res_3_e() { return res_i_r(3, reg.e); }
    tick_t res_4_e() { return res_i_r(4, reg.e); }
    tick_t res_5_e() { return res_i_r(5, reg.e); }
    tick_t res_6_e() { return res_i_r(6, reg.e); }
    tick_t res_7_e() { return res_i_r(7, reg.e); }

    tick_t res_0_h() { return res_i_r(0, reg.h); }
    tick_t res_1_h() { return res_i_r(1, reg.h); }
    tick_t res_2_h() { return res_i_r(2, reg.h); }
    tick_t res_3_h() { return res_i_r(3, reg.h); }
    tick_t res_4_h() { return res_i_r(4, reg.h); }
    tick_t res_5_h() { return res_i_r(5, reg.h); }
    tick_t res_6_h() { return res_i_r(6, reg.h); }
    tick_t res_7_h() { return res_i_r(7, reg.h); }

    tick_t res_0_l() { return res_i_r(0, reg.l); }
    tick_t res_1_l() { return res_i_r(1, reg.l); }
    tick_t res_2_l() { return res_i_r(2, reg.l); }
    tick_t res_3_l() { return res_i_r(3, reg.l); }
    tick_t res_4_l() { return res_i_r(4, reg.l); }
    tick_t res_5_l() { return res_i_r(5, reg.l); }
    tick_t res_6_l() { return res_i_r(6, reg.l); }
    tick_t res_7_l() { return res_i_r(7, reg.l); }

    tick_t res_0_phl() { return res_i_phl(0); }
    tick_t res_1_phl() { return res_i_phl(1); }
    tick_t res_2_phl() { return res_i_phl(2); }
    tick_t res_3_phl() { return res_i_phl(3); }
    tick_t res_4_phl() { return res_i_phl(4); }
    tick_t res_5_phl() { return res_i_phl(5); }
    tick_t res_6_phl() { return res_i_phl(6); }
    tick_t res_7_phl() { return res_i_phl(7); }

};

#endif
