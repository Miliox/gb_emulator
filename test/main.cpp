/*
 * test/main.cpp
 * Copyright (C) 2016 Emiliano Firmino <emiliano.firmino@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "cpu.hpp"

TEST_CASE("Z80 Constructor", "[Z80]") {
    Z80 z80;

    REQUIRE(z80.reg.a == 0);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 0);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 0);
    REQUIRE(z80.clock.t == 0);
}


TEST_CASE("Reset Z80", "[Z80]") {
    Z80 z80;

    // dirty internal registers
    z80.reg.a = 1;
    z80.reg.b = 2;
    z80.reg.c = 3;
    z80.reg.d = 4;
    z80.reg.e = 5;
    z80.reg.f = 6;
    z80.reg.h = 7;
    z80.reg.l = 8;

    // dirty clock
    z80.clock.m = 50;
    z80.clock.t = 200;

    z80.reset();

    REQUIRE(z80.reg.a == 0);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 0);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 0);
    REQUIRE(z80.clock.t == 0);
}

TEST_CASE("Z80 NOP", "[Z80]") {
    Z80 z80;
    z80.nop();

    REQUIRE(z80.reg.a == 0);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 0);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 1);
    REQUIRE(z80.clock.t == 4);
}

TEST_CASE("Z80 ADD A,E", "[Z80]") {
    Z80 z80;
    z80.reg.a = 1;
    z80.reg.e = 1;
    z80.add_a_e();

    REQUIRE(z80.reg.a == 2);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 1);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 1);
    REQUIRE(z80.clock.t == 4);

    // Zero Flag
    z80.reg.a = 0;
    z80.reg.e = 0;
    z80.add_a_e();

    REQUIRE(z80.reg.a == 0);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 0);
    REQUIRE(z80.reg.f == (uint16_t) 0x80);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 2);
    REQUIRE(z80.clock.t == 8);

    // Carry Flag
    z80.reg.a = 128;
    z80.reg.e = 200;
    z80.add_a_e();

    REQUIRE(z80.reg.a == 72);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 200);
    REQUIRE(z80.reg.f == (uint16_t) 0x10);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 3);
    REQUIRE(z80.clock.t == 12);

    // Zero + Carry Flag
    z80.reg.a = 128;
    z80.reg.e = 128;
    z80.add_a_e();

    REQUIRE(z80.reg.a == 0);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 128);
    REQUIRE(z80.reg.f == (uint16_t) 0x90);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 4);
    REQUIRE(z80.clock.t == 16);
}

TEST_CASE("Increment Registers from Z80", "[Z80]") {
    Z80 z80;

    z80.inc_a();
    REQUIRE(z80.reg.a == 1);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 0);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 1);
    REQUIRE(z80.clock.t == 4);

    z80.inc_a();
    REQUIRE(z80.reg.a == 2);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 0);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 2);
    REQUIRE(z80.clock.t == 8);

    z80.inc_e();
    REQUIRE(z80.reg.a == 2);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 1);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 3);
    REQUIRE(z80.clock.t == 12);

    z80.inc_l();
    REQUIRE(z80.reg.a == 2);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 1);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 1);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 4);
    REQUIRE(z80.clock.t == 16);
}

TEST_CASE("Decrement Registers from Z80", "[Z80]") {
    Z80 z80;

    z80.inc_a();
    REQUIRE(z80.reg.a == 1);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 0);
    REQUIRE(z80.reg.f == 0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 1);
    REQUIRE(z80.clock.t == 4);

    z80.dec_a();
    REQUIRE(z80.reg.a == 0);
    REQUIRE(z80.reg.b == 0);
    REQUIRE(z80.reg.c == 0);
    REQUIRE(z80.reg.d == 0);
    REQUIRE(z80.reg.e == 0);
    REQUIRE(z80.reg.f == 0xC0);
    REQUIRE(z80.reg.h == 0);
    REQUIRE(z80.reg.l == 0);
    REQUIRE(z80.reg.sp == 0);
    REQUIRE(z80.reg.pc == 0);
    REQUIRE(z80.clock.m == 2);
    REQUIRE(z80.clock.t == 8);
}
