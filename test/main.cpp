/*
 * test/main.cpp
 * Copyright (C) 2016 Emiliano Firmino <emiliano.firmino@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#define CATCH_CONFIG_MAIN

#define CPU_TEST "[GBCPU]"

#include "catch.hpp"
#include "cpu.hpp"

TEST_CASE("GBCPU Constructor", "[GBCPU]") {
    GBMMU mmu;
    GBCPU cpu(mmu);

    REQUIRE(cpu.reg.a == 0);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 0);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);
}


TEST_CASE("Reset GBCPU", "[GBCPU]") {
    GBMMU mmu;
    GBCPU cpu(mmu);

    // dirty internal registers
    cpu.reg.a = 1;
    cpu.reg.b = 2;
    cpu.reg.c = 3;
    cpu.reg.d = 4;
    cpu.reg.e = 5;
    cpu.reg.f = 6;
    cpu.reg.h = 7;
    cpu.reg.l = 8;

    cpu.reset();

    REQUIRE(cpu.reg.a == 0);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 0);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);
}

TEST_CASE("GBCPU NOP", "[GBCPU]") {
    GBMMU mmu;
    GBCPU cpu(mmu);

    cpu.nop();
    REQUIRE(cpu.reg.a == 0);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 0);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);
}

TEST_CASE("GBCPU ADD A,E", "[GBCPU]") {
    GBMMU mmu;
    GBCPU cpu(mmu);

    cpu.reg.a = 1;
    cpu.reg.e = 1;
    cpu.add_a_e();

    REQUIRE(cpu.reg.a == 2);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 1);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);

    // Zero Flag
    cpu.reg.a = 0;
    cpu.reg.e = 0;
    cpu.add_a_e();

    REQUIRE(cpu.reg.a == 0);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 0);
    REQUIRE(cpu.reg.f == (uint16_t) 0x80);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);

    // Carry Flag
    cpu.reg.a = 128;
    cpu.reg.e = 200;
    cpu.add_a_e();

    REQUIRE(cpu.reg.a == 72);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 200);
    REQUIRE(cpu.reg.f == (uint16_t) 0x10);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);

    // Zero + Carry Flag
    cpu.reg.a = 128;
    cpu.reg.e = 128;
    cpu.add_a_e();

    REQUIRE(cpu.reg.a == 0);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 128);
    REQUIRE(cpu.reg.f == (uint16_t) 0x90);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);
}

TEST_CASE("Increment Registers from GBCPU", "[GBCPU]") {
    GBMMU mmu;
    GBCPU cpu(mmu);

    cpu.inc_a();
    REQUIRE(cpu.reg.a == 1);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 0);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);

    cpu.inc_a();
    REQUIRE(cpu.reg.a == 2);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 0);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);

    cpu.inc_e();
    REQUIRE(cpu.reg.a == 2);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 1);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);

    cpu.inc_l();
    REQUIRE(cpu.reg.a == 2);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 1);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 1);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);
}

TEST_CASE("Decrement Registers from GBCPU", "[GBCPU]") {
    GBMMU mmu;
    GBCPU cpu(mmu);

    cpu.inc_a();
    REQUIRE(cpu.reg.a == 1);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 0);
    REQUIRE(cpu.reg.f == 0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);

    cpu.dec_a();
    REQUIRE(cpu.reg.a == 0);
    REQUIRE(cpu.reg.b == 0);
    REQUIRE(cpu.reg.c == 0);
    REQUIRE(cpu.reg.d == 0);
    REQUIRE(cpu.reg.e == 0);
    REQUIRE(cpu.reg.f == 0xC0);
    REQUIRE(cpu.reg.h == 0);
    REQUIRE(cpu.reg.l == 0);
    REQUIRE(cpu.reg.sp == 0);
    REQUIRE(cpu.reg.pc == 0);
}

TEST_CASE("PUSH/POP GBCPU", "[GBCPU]") {
    GBMMU mmu;
    GBCPU cpu(mmu);

    cpu.reg.sp = 0xfffe; // init stack

    cpu.reg.b = 0x10;
    cpu.reg.c = 0x08;
    cpu.push_bc();

    cpu.reg.b = 0xff;
    cpu.reg.c = 0xff;
    cpu.pop_bc();

    REQUIRE(cpu.reg.c == 0x08);
    REQUIRE(cpu.reg.b == 0x10);
}

TEST_CASE("LD", CPU_TEST) {
    GBMMU mmu;
    GBCPU cpu(mmu);

    SECTION( "LD A,r" ) {
        cpu.reg.a = 0;
        cpu.reg.b = 1;
        cpu.reg.c = 2;
        cpu.reg.d = 3;
        cpu.reg.e = 4;
        cpu.reg.h = 5;
        cpu.reg.l = 6;

        REQUIRE(cpu.ld_a_a() == 4);
        REQUIRE(cpu.reg.a == 0);
        REQUIRE(cpu.ld_a_b() == 4);
        REQUIRE(cpu.reg.a == 1);
        REQUIRE(cpu.ld_a_c() == 4);
        REQUIRE(cpu.reg.a == 2);
        REQUIRE(cpu.ld_a_d() == 4);
        REQUIRE(cpu.reg.a == 3);
        REQUIRE(cpu.ld_a_e() == 4);
        REQUIRE(cpu.reg.a == 4);
        REQUIRE(cpu.ld_a_h() == 4);
        REQUIRE(cpu.reg.a == 5);
        REQUIRE(cpu.ld_a_l() == 4);
        REQUIRE(cpu.reg.a == 6);
    }

    SECTION( "LD B,r" ) {
        cpu.reg.a = 10;
        cpu.reg.b = 11;
        cpu.reg.c = 12;
        cpu.reg.d = 13;
        cpu.reg.e = 14;
        cpu.reg.h = 15;
        cpu.reg.l = 16;

        REQUIRE(cpu.ld_b_b() == 4);
        REQUIRE(cpu.reg.b == 11);
        REQUIRE(cpu.ld_b_a() == 4);
        REQUIRE(cpu.reg.b == 10);
        REQUIRE(cpu.ld_b_c() == 4);
        REQUIRE(cpu.reg.b == 12);
        REQUIRE(cpu.ld_b_d() == 4);
        REQUIRE(cpu.reg.b == 13);
        REQUIRE(cpu.ld_b_e() == 4);
        REQUIRE(cpu.reg.b == 14);
        REQUIRE(cpu.ld_b_h() == 4);
        REQUIRE(cpu.reg.b == 15);
        REQUIRE(cpu.ld_b_l() == 4);
        REQUIRE(cpu.reg.b == 16);
    }

    SECTION( "LD C,r" ) {
        cpu.reg.a = 20;
        cpu.reg.b = 21;
        cpu.reg.c = 22;
        cpu.reg.d = 23;
        cpu.reg.e = 24;
        cpu.reg.h = 25;
        cpu.reg.l = 26;

        REQUIRE(cpu.ld_c_c() == 4);
        REQUIRE(cpu.reg.c == 22);
        REQUIRE(cpu.ld_c_a() == 4);
        REQUIRE(cpu.reg.c == 20);
        REQUIRE(cpu.ld_c_b() == 4);
        REQUIRE(cpu.reg.c == 21);
        REQUIRE(cpu.ld_c_d() == 4);
        REQUIRE(cpu.reg.c == 23);
        REQUIRE(cpu.ld_c_e() == 4);
        REQUIRE(cpu.reg.c == 24);
        REQUIRE(cpu.ld_c_h() == 4);
        REQUIRE(cpu.reg.c == 25);
        REQUIRE(cpu.ld_c_l() == 4);
        REQUIRE(cpu.reg.c == 26);
    }

    SECTION( "LD D,r" ) {
        cpu.reg.a = 30;
        cpu.reg.b = 31;
        cpu.reg.c = 32;
        cpu.reg.d = 33;
        cpu.reg.e = 34;
        cpu.reg.h = 35;
        cpu.reg.l = 36;

        REQUIRE(cpu.ld_d_d() == 4);
        REQUIRE(cpu.reg.d == 33);
        REQUIRE(cpu.ld_d_a() == 4);
        REQUIRE(cpu.reg.d == 30);
        REQUIRE(cpu.ld_d_b() == 4);
        REQUIRE(cpu.reg.d == 31);
        REQUIRE(cpu.ld_d_c() == 4);
        REQUIRE(cpu.reg.d == 32);
        REQUIRE(cpu.ld_d_e() == 4);
        REQUIRE(cpu.reg.d == 34);
        REQUIRE(cpu.ld_d_h() == 4);
        REQUIRE(cpu.reg.d == 35);
        REQUIRE(cpu.ld_d_l() == 4);
        REQUIRE(cpu.reg.d == 36);
    }

    SECTION( "LD E,r" ) {
        cpu.reg.a = 40;
        cpu.reg.b = 41;
        cpu.reg.c = 42;
        cpu.reg.d = 43;
        cpu.reg.e = 44;
        cpu.reg.h = 45;
        cpu.reg.l = 46;

        REQUIRE(cpu.ld_e_e() == 4);
        REQUIRE(cpu.reg.e == 44);
        REQUIRE(cpu.ld_e_a() == 4);
        REQUIRE(cpu.reg.e == 40);
        REQUIRE(cpu.ld_e_b() == 4);
        REQUIRE(cpu.reg.e == 41);
        REQUIRE(cpu.ld_e_c() == 4);
        REQUIRE(cpu.reg.e == 42);
        REQUIRE(cpu.ld_e_d() == 4);
        REQUIRE(cpu.reg.e == 43);
        REQUIRE(cpu.ld_e_h() == 4);
        REQUIRE(cpu.reg.e == 45);
        REQUIRE(cpu.ld_e_l() == 4);
        REQUIRE(cpu.reg.e == 46);
    }

    SECTION( "LD H,r" ) {
        cpu.reg.a = 50;
        cpu.reg.b = 51;
        cpu.reg.c = 52;
        cpu.reg.d = 53;
        cpu.reg.e = 54;
        cpu.reg.h = 55;
        cpu.reg.l = 56;

        REQUIRE(cpu.ld_h_h() == 4);
        REQUIRE(cpu.reg.h == 55);
        REQUIRE(cpu.ld_h_a() == 4);
        REQUIRE(cpu.reg.h == 50);
        REQUIRE(cpu.ld_h_b() == 4);
        REQUIRE(cpu.reg.h == 51);
        REQUIRE(cpu.ld_h_c() == 4);
        REQUIRE(cpu.reg.h == 52);
        REQUIRE(cpu.ld_h_d() == 4);
        REQUIRE(cpu.reg.h == 53);
        REQUIRE(cpu.ld_h_e() == 4);
        REQUIRE(cpu.reg.h == 54);
        REQUIRE(cpu.ld_h_l() == 4);
        REQUIRE(cpu.reg.h == 56);
    }

    SECTION( "LD L,r" ) {
        cpu.reg.a = 60;
        cpu.reg.b = 61;
        cpu.reg.c = 62;
        cpu.reg.d = 63;
        cpu.reg.e = 64;
        cpu.reg.h = 65;
        cpu.reg.l = 66;

        REQUIRE(cpu.ld_h_h() == 4);
        REQUIRE(cpu.reg.h == 65);
        REQUIRE(cpu.ld_h_a() == 4);
        REQUIRE(cpu.reg.h == 60);
        REQUIRE(cpu.ld_h_b() == 4);
        REQUIRE(cpu.reg.h == 61);
        REQUIRE(cpu.ld_h_c() == 4);
        REQUIRE(cpu.reg.h == 62);
        REQUIRE(cpu.ld_h_d() == 4);
        REQUIRE(cpu.reg.h == 63);
        REQUIRE(cpu.ld_h_e() == 4);
        REQUIRE(cpu.reg.h == 64);
        REQUIRE(cpu.ld_h_l() == 4);
        REQUIRE(cpu.reg.h == 66);
    }

    SECTION( "LD L,r" ) {
        cpu.reg.a = 70;
        cpu.reg.b = 71;
        cpu.reg.c = 72;
        cpu.reg.d = 73;
        cpu.reg.e = 74;
        cpu.reg.h = 75;
        cpu.reg.l = 76;

        REQUIRE(cpu.ld_l_l() == 4);
        REQUIRE(cpu.reg.l == 76);
        REQUIRE(cpu.ld_l_a() == 4);
        REQUIRE(cpu.reg.l == 70);
        REQUIRE(cpu.ld_l_b() == 4);
        REQUIRE(cpu.reg.l == 71);
        REQUIRE(cpu.ld_l_c() == 4);
        REQUIRE(cpu.reg.l == 72);
        REQUIRE(cpu.ld_l_d() == 4);
        REQUIRE(cpu.reg.l == 73);
        REQUIRE(cpu.ld_l_e() == 4);
        REQUIRE(cpu.reg.l == 74);
        REQUIRE(cpu.ld_l_h() == 4);
        REQUIRE(cpu.reg.l == 75);
    }
}

TEST_CASE("ADD", CPU_TEST) {
    GBMMU mmu;
    GBCPU cpu(mmu);

    SECTION( "ADD A,r" ) {
        cpu.reg.a = 1;
        cpu.reg.b = 5;
        cpu.reg.c = 10;
        cpu.reg.d = 15;
        cpu.reg.e = 20;
        cpu.reg.h = 25;
        cpu.reg.l = 30;

        REQUIRE(cpu.add_a_a() == 4);
        REQUIRE(cpu.reg.a == 2);
        REQUIRE(cpu.add_a_b() == 4);
        REQUIRE(cpu.reg.a == 7);
        REQUIRE(cpu.add_a_c() == 4);
        REQUIRE(cpu.reg.a == 17);
        REQUIRE(cpu.add_a_d() == 4);
        REQUIRE(cpu.reg.a == 32);
        REQUIRE(cpu.add_a_e() == 4);
        REQUIRE(cpu.reg.a == 52);
        REQUIRE(cpu.add_a_h() == 4);
        REQUIRE(cpu.reg.a == 77);
        REQUIRE(cpu.add_a_l() == 4);
        REQUIRE(cpu.reg.a == 107);
    }

    SECTION( "ADD CARRY" ) {
        cpu.reg.a = 107;
        // Force Half-Carry
        REQUIRE(cpu.add_a_r(21) == 4);
        REQUIRE(cpu.reg.a == 128);
        REQUIRE(cpu.reg.f == 0x20);

        // No Carry
        REQUIRE(cpu.add_a_r(1) == 4);
        REQUIRE(cpu.reg.a == 129);
        REQUIRE(cpu.reg.f == 0x00);

        // Force Carry, Half-Carry and Zero
        REQUIRE(cpu.add_a_r(127) == 4);
        REQUIRE(cpu.reg.a == 0);
        REQUIRE(cpu.reg.f == 0xb0);

    }
}
