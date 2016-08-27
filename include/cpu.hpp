#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>

#include "clock.hpp"
#include "mmu.hpp"

class Registers {
public:
    uint8_t a; // accumulator
    uint8_t f; // flag

    uint8_t b;
    uint8_t c;

    uint8_t d;
    uint8_t e;

    uint8_t h;
    uint8_t l;

    uint16_t pc;  // program counter
    uint16_t sp;  // stack pointer

    Registers() : a(0), f(0), b(0), c(0), d(0), e(0), h(0), l (0), pc(0), sp(0) {}
};

class Z80 {
private:
    int32_t acc;

    void init_instrunction_map();
    void init_cb_instrunction_map();
public:
    MMU mmu;
    Clock clock; // global system clock
    Registers reg;

    bool ime; // interrupt master enable flag

    std::vector<void (Z80::*)()> instruction_map;
    std::vector<void (Z80::*)()> cb_instruction_map;

    Z80();
    void reset();

    // Common Instruction Behavior
    void ld_r_r(uint8_t&, const uint8_t&);
    void ld_r_prr(uint8_t&, const uint8_t&, const uint8_t&);
    void ld_prr_r(const uint8_t&, const uint8_t&, const uint8_t&);
    void ld_r_n(uint8_t&);
    void ld_rr_nn(uint8_t&, uint8_t&);
    void push_rr(const uint8_t&, const uint8_t&);
    void pop_rr(uint8_t&, uint8_t&);
    void add_a_r(const uint8_t&);
    void adc_a_r(const uint8_t&);
    void add_hl_rr(const uint8_t&, const uint8_t&);
    void adc_hl_rr(const uint8_t&, const uint8_t&);
    void sub(const uint8_t&);
    void sbc_a_r(const uint8_t&);
    void and_r(const uint8_t&);
    void or_r(const uint8_t&);
    void xor_r(const uint8_t&);
    void cp_r(const uint8_t&);
    void inc_r(uint8_t&);
    void dec_r(uint8_t&);
    void swap_r(uint8_t&);
    void inc_rr(uint8_t&, uint8_t&);
    void dec_rr(uint8_t&, uint8_t&);
    void rst(const uint16_t addr);

    void bit_i_r(const uint8_t index, const uint8_t& r);
    void set_i_r(const uint8_t index, uint8_t& r);
    void res_i_r(const uint8_t index, uint8_t& r);
    void rlc_r(uint8_t& r);
    void rrc_r(uint8_t& r);
    void rl_r(uint8_t& r);
    void rr_r(uint8_t& r);
    void sla_r(uint8_t& r);
    void sra_r(uint8_t& r);

    void bit_i_phl(const uint8_t index);
    void set_i_phl(const uint8_t index);
    void res_i_phl(const uint8_t index);
    void rlc_phl();
    void rrc_phl();
    void rl_phl();
    void rr_phl();
    void sla_phl();
    void sra_phl();

    void not_implemented_error() {
        throw std::runtime_error("Instruction Not Yet Implemented");
    }

    void not_supported_error() {
        throw std::runtime_error("Instruction Not Supported");
    }

    void cb_branch() {
        uint8_t op = mmu.read_byte(reg.pc++);
        (*this.*cb_instruction_map[op])();
    }

    // Instruction Set
    void nop();
    void halt();
    void stop();
    void rlca();
    void rla();
    void rrca();
    void rra();
    void cpl();
    void scf();
    void ccf();
    void di();
    void ei();

    void ld_a_a() { ld_r_r(reg.a, reg.a); }
    void ld_a_b() { ld_r_r(reg.a, reg.b); }
    void ld_a_c() { ld_r_r(reg.a, reg.c); }
    void ld_a_d() { ld_r_r(reg.a, reg.d); }
    void ld_a_e() { ld_r_r(reg.a, reg.e); }
    void ld_a_h() { ld_r_r(reg.a, reg.h); }
    void ld_a_l() { ld_r_r(reg.a, reg.l); }
    void ld_a_phl() { ld_r_prr(reg.a, reg.h, reg.l); }

    void ld_b_a() { ld_r_r(reg.b, reg.a); }
    void ld_b_b() { ld_r_r(reg.b, reg.b); }
    void ld_b_c() { ld_r_r(reg.b, reg.c); }
    void ld_b_d() { ld_r_r(reg.b, reg.d); }
    void ld_b_e() { ld_r_r(reg.b, reg.e); }
    void ld_b_h() { ld_r_r(reg.b, reg.h); }
    void ld_b_l() { ld_r_r(reg.b, reg.l); }
    void ld_b_phl() { ld_r_prr(reg.b, reg.h, reg.l); }

    void ld_c_a() { ld_r_r(reg.c, reg.a); }
    void ld_c_b() { ld_r_r(reg.c, reg.b); }
    void ld_c_c() { ld_r_r(reg.c, reg.c); }
    void ld_c_d() { ld_r_r(reg.c, reg.d); }
    void ld_c_e() { ld_r_r(reg.c, reg.e); }
    void ld_c_h() { ld_r_r(reg.c, reg.h); }
    void ld_c_l() { ld_r_r(reg.c, reg.l); }
    void ld_c_phl() { ld_r_prr(reg.c, reg.h, reg.l); }

    void ld_d_a() { ld_r_r(reg.d, reg.a); }
    void ld_d_b() { ld_r_r(reg.d, reg.b); }
    void ld_d_c() { ld_r_r(reg.d, reg.c); }
    void ld_d_d() { ld_r_r(reg.d, reg.d); }
    void ld_d_e() { ld_r_r(reg.d, reg.e); }
    void ld_d_h() { ld_r_r(reg.d, reg.h); }
    void ld_d_l() { ld_r_r(reg.d, reg.l); }
    void ld_d_phl() { ld_r_prr(reg.d, reg.h, reg.l); }

    void ld_e_a() { ld_r_r(reg.e, reg.a); }
    void ld_e_b() { ld_r_r(reg.e, reg.b); }
    void ld_e_c() { ld_r_r(reg.e, reg.c); }
    void ld_e_d() { ld_r_r(reg.e, reg.d); }
    void ld_e_e() { ld_r_r(reg.e, reg.e); }
    void ld_e_h() { ld_r_r(reg.e, reg.h); }
    void ld_e_l() { ld_r_r(reg.e, reg.l); }
    void ld_e_phl() { ld_r_prr(reg.e, reg.h, reg.l); }

    void ld_h_a() { ld_r_r(reg.h, reg.a); }
    void ld_h_b() { ld_r_r(reg.h, reg.b); }
    void ld_h_c() { ld_r_r(reg.h, reg.c); }
    void ld_h_d() { ld_r_r(reg.h, reg.d); }
    void ld_h_e() { ld_r_r(reg.h, reg.e); }
    void ld_h_h() { ld_r_r(reg.h, reg.h); }
    void ld_h_l() { ld_r_r(reg.h, reg.l); }
    void ld_h_phl() { ld_r_prr(reg.h, reg.h, reg.l); }

    void ld_l_a() { ld_r_r(reg.l, reg.a); }
    void ld_l_b() { ld_r_r(reg.l, reg.b); }
    void ld_l_c() { ld_r_r(reg.l, reg.c); }
    void ld_l_d() { ld_r_r(reg.l, reg.d); }
    void ld_l_e() { ld_r_r(reg.l, reg.e); }
    void ld_l_h() { ld_r_r(reg.l, reg.h); }
    void ld_l_l() { ld_r_r(reg.l, reg.l); }
    void ld_l_phl() { ld_r_prr(reg.l, reg.h, reg.l); }

    void ld_sp_hl();

    void ld_hl_spn();

    void ld_phl_a() { ld_prr_r(reg.h, reg.l, reg.a); }
    void ld_phl_b() { ld_prr_r(reg.h, reg.l, reg.b); }
    void ld_phl_c() { ld_prr_r(reg.h, reg.l, reg.c); }
    void ld_phl_d() { ld_prr_r(reg.h, reg.l, reg.d); }
    void ld_phl_e() { ld_prr_r(reg.h, reg.l, reg.e); }
    void ld_phl_h() { ld_prr_r(reg.h, reg.l, reg.h); }
    void ld_phl_l() { ld_prr_r(reg.h, reg.l, reg.l); }
    void ld_phl_n();

    void ld_pbc_a() { ld_prr_r(reg.b, reg.c, reg.a); }
    void ld_pde_a() { ld_prr_r(reg.d, reg.e, reg.a); }

    void ld_a_pbc() { ld_r_prr(reg.a, reg.b, reg.c); }
    void ld_a_pde() { ld_r_prr(reg.a, reg.d, reg.e); }
    void ld_a_pnn();

    void ld_pnn_a();

    void ld_a_n() { ld_r_n(reg.a); }
    void ld_b_n() { ld_r_n(reg.b); }
    void ld_c_n() { ld_r_n(reg.c); }
    void ld_d_n() { ld_r_n(reg.d); }
    void ld_e_n() { ld_r_n(reg.e); }
    void ld_h_n() { ld_r_n(reg.h); }
    void ld_l_n() { ld_r_n(reg.l); }

    void ld_bc_nn() { ld_rr_nn(reg.b, reg.c); }
    void ld_de_nn() { ld_rr_nn(reg.d, reg.e); }
    void ld_hl_nn() { ld_rr_nn(reg.h, reg.l); }
    void ld_sp_nn();

    void ld_pnn_sp();

    void ld_a_offc();
    void ld_offc_a();

    void ldh_offn_a();
    void ldh_a_offn();

    void ldi_a_phl();
    void ldi_phl_a();

    void ldd_a_phl();
    void ldd_phl_a();

    void add_a_a() { add_a_r(reg.a); }
    void add_a_b() { add_a_r(reg.b); }
    void add_a_c() { add_a_r(reg.c); }
    void add_a_d() { add_a_r(reg.d); }
    void add_a_e() { add_a_r(reg.e); }
    void add_a_h() { add_a_r(reg.h); }
    void add_a_l() { add_a_r(reg.l); }
    void add_a_n();
    void add_a_phl();

    void add_sp_n();

    void adc_a_a() { adc_a_r(reg.a); }
    void adc_a_b() { adc_a_r(reg.b); }
    void adc_a_c() { adc_a_r(reg.c); }
    void adc_a_d() { adc_a_r(reg.d); }
    void adc_a_e() { adc_a_r(reg.e); }
    void adc_a_h() { adc_a_r(reg.h); }
    void adc_a_l() { adc_a_r(reg.l); }
    void adc_a_n();
    void adc_a_phl();

    void add_hl_bc() { add_hl_rr(reg.b, reg.c); }
    void add_hl_de() { add_hl_rr(reg.d, reg.e); }
    void add_hl_hl() { add_hl_rr(reg.h, reg.l); }
    void add_hl_sp();

    void sub_a() { sub(reg.a); }
    void sub_b() { sub(reg.b); }
    void sub_c() { sub(reg.c); }
    void sub_d() { sub(reg.d); }
    void sub_e() { sub(reg.e); }
    void sub_h() { sub(reg.h); }
    void sub_l() { sub(reg.l); }
    void sub_n();
    void sub_phl();

    void sbc_a_a() { sbc_a_r(reg.a); }
    void sbc_a_b() { sbc_a_r(reg.b); }
    void sbc_a_c() { sbc_a_r(reg.c); }
    void sbc_a_d() { sbc_a_r(reg.d); }
    void sbc_a_e() { sbc_a_r(reg.e); }
    void sbc_a_h() { sbc_a_r(reg.h); }
    void sbc_a_l() { sbc_a_r(reg.l); }
    void sbc_a_n();
    void sbc_a_phl();

    void and_a() { and_r(reg.a); }
    void and_b() { and_r(reg.b); }
    void and_c() { and_r(reg.c); }
    void and_d() { and_r(reg.d); }
    void and_e() { and_r(reg.e); }
    void and_h() { and_r(reg.h); }
    void and_l() { and_r(reg.l); }
    void and_n();
    void and_phl();

    void or_a() { or_r(reg.a); }
    void or_b() { or_r(reg.b); }
    void or_c() { or_r(reg.c); }
    void or_d() { or_r(reg.d); }
    void or_e() { or_r(reg.e); }
    void or_h() { or_r(reg.h); }
    void or_l() { or_r(reg.l); }
    void or_n();
    void or_phl();

    void xor_a() { xor_r(reg.a); }
    void xor_b() { xor_r(reg.b); }
    void xor_c() { xor_r(reg.c); }
    void xor_d() { xor_r(reg.d); }
    void xor_e() { xor_r(reg.e); }
    void xor_h() { xor_r(reg.h); }
    void xor_l() { xor_r(reg.l); }
    void xor_n();
    void xor_phl();

    void cp_a() { cp_r(reg.a); }
    void cp_b() { cp_r(reg.b); }
    void cp_c() { cp_r(reg.c); }
    void cp_d() { cp_r(reg.d); }
    void cp_e() { cp_r(reg.e); }
    void cp_h() { cp_r(reg.h); }
    void cp_l() { cp_r(reg.l); }
    void cp_n();
    void cp_phl();

    void inc_a() { inc_r(reg.a); };
    void inc_b() { inc_r(reg.b); };
    void inc_c() { inc_r(reg.c); };
    void inc_d() { inc_r(reg.d); };
    void inc_e() { inc_r(reg.e); };
    void inc_h() { inc_r(reg.h); };
    void inc_l() { inc_r(reg.l); };

    void dec_a() { dec_r(reg.a); }
    void dec_b() { dec_r(reg.b); }
    void dec_c() { dec_r(reg.c); }
    void dec_d() { dec_r(reg.d); }
    void dec_e() { dec_r(reg.e); }
    void dec_h() { dec_r(reg.h); }
    void dec_l() { dec_r(reg.l); }

    void swap_a() { swap_r(reg.a); }
    void swap_b() { swap_r(reg.b); }
    void swap_c() { swap_r(reg.c); }
    void swap_d() { swap_r(reg.d); }
    void swap_e() { swap_r(reg.e); }
    void swap_h() { swap_r(reg.h); }
    void swap_l() { swap_r(reg.l); }
    void swap_phl();

    void inc_bc() { inc_rr(reg.b, reg.c); }
    void inc_de() { inc_rr(reg.d, reg.e); }
    void inc_hl() { inc_rr(reg.h, reg.l); }
    void inc_sp() { reg.sp += 1; clock += Clock(2); }
    void inc_phl();

    void dec_bc() { dec_rr(reg.b, reg.c); }
    void dec_de() { dec_rr(reg.d, reg.e); }
    void dec_hl() { dec_rr(reg.h, reg.l); }
    void dec_sp() { reg.sp -= 1; clock += Clock(2); }
    void dec_phl();

    void push_af() { push_rr(reg.a, reg.f); }
    void push_bc() { push_rr(reg.b, reg.c); }
    void push_de() { push_rr(reg.d, reg.e); }
    void push_hl() { push_rr(reg.h, reg.l); }

    void pop_af() { pop_rr(reg.a, reg.f); }
    void pop_bc() { pop_rr(reg.b, reg.c); }
    void pop_de() { pop_rr(reg.d, reg.e); }
    void pop_hl() { pop_rr(reg.h, reg.l); }

    void rst_00() { rst(0x00); };
    void rst_08() { rst(0x08); };
    void rst_10() { rst(0x10); };
    void rst_18() { rst(0x18); };
    void rst_20() { rst(0x20); };
    void rst_28() { rst(0x28); };
    void rst_30() { rst(0x30); };
    void rst_38() { rst(0x38); };

    void call();
    void call_z();
    void call_nz();
    void call_c();
    void call_nc();

    void ret();
    void reti();
    void ret_z();
    void ret_nz();
    void ret_c();
    void ret_nc();

    void jp();
    void jp_z();
    void jp_nz();
    void jp_c();
    void jp_nc();
    void jp_hl();

    void jr();
    void jr_z();
    void jr_nz();
    void jr_c();
    void jr_nc();

    // CB Instruction Set

    void rlc_a() { rlc_r(reg.a); }
    void rlc_b() { rlc_r(reg.b); }
    void rlc_c() { rlc_r(reg.c); }
    void rlc_d() { rlc_r(reg.d); }
    void rlc_e() { rlc_r(reg.e); }
    void rlc_h() { rlc_r(reg.h); }
    void rlc_l() { rlc_r(reg.l); }

    void rrc_a() { rrc_r(reg.a); }
    void rrc_b() { rrc_r(reg.b); }
    void rrc_c() { rrc_r(reg.c); }
    void rrc_d() { rrc_r(reg.d); }
    void rrc_e() { rrc_r(reg.e); }
    void rrc_h() { rrc_r(reg.h); }
    void rrc_l() { rrc_r(reg.l); }

    void rl_a() { rl_r(reg.a); }
    void rl_b() { rl_r(reg.b); }
    void rl_c() { rl_r(reg.c); }
    void rl_d() { rl_r(reg.d); }
    void rl_e() { rl_r(reg.e); }
    void rl_h() { rl_r(reg.h); }
    void rl_l() { rl_r(reg.l); }

    void rr_a() { rr_r(reg.a); }
    void rr_b() { rr_r(reg.b); }
    void rr_c() { rr_r(reg.c); }
    void rr_d() { rr_r(reg.d); }
    void rr_e() { rr_r(reg.e); }
    void rr_h() { rr_r(reg.h); }
    void rr_l() { rr_r(reg.l); }

    void sla_a() { sla_r(reg.a); }
    void sla_b() { sla_r(reg.b); }
    void sla_c() { sla_r(reg.c); }
    void sla_d() { sla_r(reg.d); }
    void sla_e() { sla_r(reg.e); }
    void sla_h() { sla_r(reg.h); }
    void sla_l() { sla_r(reg.l); }

    void sra_a() { sra_r(reg.a); }
    void sra_b() { sra_r(reg.b); }
    void sra_c() { sra_r(reg.c); }
    void sra_d() { sra_r(reg.d); }
    void sra_e() { sra_r(reg.e); }
    void sra_h() { sra_r(reg.h); }
    void sra_l() { sra_r(reg.l); }

    void bit_0_a() { bit_i_r(0, reg.a); }
    void bit_1_a() { bit_i_r(1, reg.a); }
    void bit_2_a() { bit_i_r(2, reg.a); }
    void bit_3_a() { bit_i_r(3, reg.a); }
    void bit_4_a() { bit_i_r(4, reg.a); }
    void bit_5_a() { bit_i_r(5, reg.a); }
    void bit_6_a() { bit_i_r(6, reg.a); }
    void bit_7_a() { bit_i_r(7, reg.a); }

    void bit_0_b() { bit_i_r(0, reg.b); }
    void bit_1_b() { bit_i_r(1, reg.b); }
    void bit_2_b() { bit_i_r(2, reg.b); }
    void bit_3_b() { bit_i_r(3, reg.b); }
    void bit_4_b() { bit_i_r(4, reg.b); }
    void bit_5_b() { bit_i_r(5, reg.b); }
    void bit_6_b() { bit_i_r(6, reg.b); }
    void bit_7_b() { bit_i_r(7, reg.b); }

    void bit_0_c() { bit_i_r(0, reg.c); }
    void bit_1_c() { bit_i_r(1, reg.c); }
    void bit_2_c() { bit_i_r(2, reg.c); }
    void bit_3_c() { bit_i_r(3, reg.c); }
    void bit_4_c() { bit_i_r(4, reg.c); }
    void bit_5_c() { bit_i_r(5, reg.c); }
    void bit_6_c() { bit_i_r(6, reg.c); }
    void bit_7_c() { bit_i_r(7, reg.c); }

    void bit_0_d() { bit_i_r(0, reg.d); }
    void bit_1_d() { bit_i_r(1, reg.d); }
    void bit_2_d() { bit_i_r(2, reg.d); }
    void bit_3_d() { bit_i_r(3, reg.d); }
    void bit_4_d() { bit_i_r(4, reg.d); }
    void bit_5_d() { bit_i_r(5, reg.d); }
    void bit_6_d() { bit_i_r(6, reg.d); }
    void bit_7_d() { bit_i_r(7, reg.d); }

    void bit_0_e() { bit_i_r(0, reg.e); }
    void bit_1_e() { bit_i_r(1, reg.e); }
    void bit_2_e() { bit_i_r(2, reg.e); }
    void bit_3_e() { bit_i_r(3, reg.e); }
    void bit_4_e() { bit_i_r(4, reg.e); }
    void bit_5_e() { bit_i_r(5, reg.e); }
    void bit_6_e() { bit_i_r(6, reg.e); }
    void bit_7_e() { bit_i_r(7, reg.e); }

    void bit_0_h() { bit_i_r(0, reg.h); }
    void bit_1_h() { bit_i_r(1, reg.h); }
    void bit_2_h() { bit_i_r(2, reg.h); }
    void bit_3_h() { bit_i_r(3, reg.h); }
    void bit_4_h() { bit_i_r(4, reg.h); }
    void bit_5_h() { bit_i_r(5, reg.h); }
    void bit_6_h() { bit_i_r(6, reg.h); }
    void bit_7_h() { bit_i_r(7, reg.h); }

    void bit_0_l() { bit_i_r(0, reg.l); }
    void bit_1_l() { bit_i_r(1, reg.l); }
    void bit_2_l() { bit_i_r(2, reg.l); }
    void bit_3_l() { bit_i_r(3, reg.l); }
    void bit_4_l() { bit_i_r(4, reg.l); }
    void bit_5_l() { bit_i_r(5, reg.l); }
    void bit_6_l() { bit_i_r(6, reg.l); }
    void bit_7_l() { bit_i_r(7, reg.l); }

    void bit_0_phl() { bit_i_phl(0); }
    void bit_1_phl() { bit_i_phl(1); }
    void bit_2_phl() { bit_i_phl(2); }
    void bit_3_phl() { bit_i_phl(3); }
    void bit_4_phl() { bit_i_phl(4); }
    void bit_5_phl() { bit_i_phl(5); }
    void bit_6_phl() { bit_i_phl(6); }
    void bit_7_phl() { bit_i_phl(7); }


    void set_0_a() { set_i_r(0, reg.a); }
    void set_1_a() { set_i_r(1, reg.a); }
    void set_2_a() { set_i_r(2, reg.a); }
    void set_3_a() { set_i_r(3, reg.a); }
    void set_4_a() { set_i_r(4, reg.a); }
    void set_5_a() { set_i_r(5, reg.a); }
    void set_6_a() { set_i_r(6, reg.a); }
    void set_7_a() { set_i_r(7, reg.a); }

    void set_0_b() { set_i_r(0, reg.b); }
    void set_1_b() { set_i_r(1, reg.b); }
    void set_2_b() { set_i_r(2, reg.b); }
    void set_3_b() { set_i_r(3, reg.b); }
    void set_4_b() { set_i_r(4, reg.b); }
    void set_5_b() { set_i_r(5, reg.b); }
    void set_6_b() { set_i_r(6, reg.b); }
    void set_7_b() { set_i_r(7, reg.b); }

    void set_0_c() { set_i_r(0, reg.c); }
    void set_1_c() { set_i_r(1, reg.c); }
    void set_2_c() { set_i_r(2, reg.c); }
    void set_3_c() { set_i_r(3, reg.c); }
    void set_4_c() { set_i_r(4, reg.c); }
    void set_5_c() { set_i_r(5, reg.c); }
    void set_6_c() { set_i_r(6, reg.c); }
    void set_7_c() { set_i_r(7, reg.c); }

    void set_0_d() { set_i_r(0, reg.d); }
    void set_1_d() { set_i_r(1, reg.d); }
    void set_2_d() { set_i_r(2, reg.d); }
    void set_3_d() { set_i_r(3, reg.d); }
    void set_4_d() { set_i_r(4, reg.d); }
    void set_5_d() { set_i_r(5, reg.d); }
    void set_6_d() { set_i_r(6, reg.d); }
    void set_7_d() { set_i_r(7, reg.d); }

    void set_0_e() { set_i_r(0, reg.e); }
    void set_1_e() { set_i_r(1, reg.e); }
    void set_2_e() { set_i_r(2, reg.e); }
    void set_3_e() { set_i_r(3, reg.e); }
    void set_4_e() { set_i_r(4, reg.e); }
    void set_5_e() { set_i_r(5, reg.e); }
    void set_6_e() { set_i_r(6, reg.e); }
    void set_7_e() { set_i_r(7, reg.e); }

    void set_0_h() { set_i_r(0, reg.h); }
    void set_1_h() { set_i_r(1, reg.h); }
    void set_2_h() { set_i_r(2, reg.h); }
    void set_3_h() { set_i_r(3, reg.h); }
    void set_4_h() { set_i_r(4, reg.h); }
    void set_5_h() { set_i_r(5, reg.h); }
    void set_6_h() { set_i_r(6, reg.h); }
    void set_7_h() { set_i_r(7, reg.h); }

    void set_0_l() { set_i_r(0, reg.l); }
    void set_1_l() { set_i_r(1, reg.l); }
    void set_2_l() { set_i_r(2, reg.l); }
    void set_3_l() { set_i_r(3, reg.l); }
    void set_4_l() { set_i_r(4, reg.l); }
    void set_5_l() { set_i_r(5, reg.l); }
    void set_6_l() { set_i_r(6, reg.l); }
    void set_7_l() { set_i_r(7, reg.l); }

    void set_0_phl() { set_i_phl(0); }
    void set_1_phl() { set_i_phl(1); }
    void set_2_phl() { set_i_phl(2); }
    void set_3_phl() { set_i_phl(3); }
    void set_4_phl() { set_i_phl(4); }
    void set_5_phl() { set_i_phl(5); }
    void set_6_phl() { set_i_phl(6); }
    void set_7_phl() { set_i_phl(7); }

    void res_0_a() { res_i_r(0, reg.a); }
    void res_1_a() { res_i_r(1, reg.a); }
    void res_2_a() { res_i_r(2, reg.a); }
    void res_3_a() { res_i_r(3, reg.a); }
    void res_4_a() { res_i_r(4, reg.a); }
    void res_5_a() { res_i_r(5, reg.a); }
    void res_6_a() { res_i_r(6, reg.a); }
    void res_7_a() { res_i_r(7, reg.a); }

    void res_0_b() { res_i_r(0, reg.b); }
    void res_1_b() { res_i_r(1, reg.b); }
    void res_2_b() { res_i_r(2, reg.b); }
    void res_3_b() { res_i_r(3, reg.b); }
    void res_4_b() { res_i_r(4, reg.b); }
    void res_5_b() { res_i_r(5, reg.b); }
    void res_6_b() { res_i_r(6, reg.b); }
    void res_7_b() { res_i_r(7, reg.b); }

    void res_0_c() { res_i_r(0, reg.c); }
    void res_1_c() { res_i_r(1, reg.c); }
    void res_2_c() { res_i_r(2, reg.c); }
    void res_3_c() { res_i_r(3, reg.c); }
    void res_4_c() { res_i_r(4, reg.c); }
    void res_5_c() { res_i_r(5, reg.c); }
    void res_6_c() { res_i_r(6, reg.c); }
    void res_7_c() { res_i_r(7, reg.c); }

    void res_0_d() { res_i_r(0, reg.d); }
    void res_1_d() { res_i_r(1, reg.d); }
    void res_2_d() { res_i_r(2, reg.d); }
    void res_3_d() { res_i_r(3, reg.d); }
    void res_4_d() { res_i_r(4, reg.d); }
    void res_5_d() { res_i_r(5, reg.d); }
    void res_6_d() { res_i_r(6, reg.d); }
    void res_7_d() { res_i_r(7, reg.d); }

    void res_0_e() { res_i_r(0, reg.e); }
    void res_1_e() { res_i_r(1, reg.e); }
    void res_2_e() { res_i_r(2, reg.e); }
    void res_3_e() { res_i_r(3, reg.e); }
    void res_4_e() { res_i_r(4, reg.e); }
    void res_5_e() { res_i_r(5, reg.e); }
    void res_6_e() { res_i_r(6, reg.e); }
    void res_7_e() { res_i_r(7, reg.e); }

    void res_0_h() { res_i_r(0, reg.h); }
    void res_1_h() { res_i_r(1, reg.h); }
    void res_2_h() { res_i_r(2, reg.h); }
    void res_3_h() { res_i_r(3, reg.h); }
    void res_4_h() { res_i_r(4, reg.h); }
    void res_5_h() { res_i_r(5, reg.h); }
    void res_6_h() { res_i_r(6, reg.h); }
    void res_7_h() { res_i_r(7, reg.h); }

    void res_0_l() { res_i_r(0, reg.l); }
    void res_1_l() { res_i_r(1, reg.l); }
    void res_2_l() { res_i_r(2, reg.l); }
    void res_3_l() { res_i_r(3, reg.l); }
    void res_4_l() { res_i_r(4, reg.l); }
    void res_5_l() { res_i_r(5, reg.l); }
    void res_6_l() { res_i_r(6, reg.l); }
    void res_7_l() { res_i_r(7, reg.l); }

    void res_0_phl() { res_i_phl(0); }
    void res_1_phl() { res_i_phl(1); }
    void res_2_phl() { res_i_phl(2); }
    void res_3_phl() { res_i_phl(3); }
    void res_4_phl() { res_i_phl(4); }
    void res_5_phl() { res_i_phl(5); }
    void res_6_phl() { res_i_phl(6); }
    void res_7_phl() { res_i_phl(7); }

};

#endif
