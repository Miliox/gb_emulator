/*
 * test/main.cpp
 * Copyright (C) 2016 Emiliano Firmino <emiliano.firmino@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#define CATCH_CONFIG_MAIN

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

    // dirty clock
    cpu.clock.m = 50;
    cpu.clock.t = 200;

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
