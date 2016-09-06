#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>

#include "cpu.hpp"
#include "gpu.hpp"
#include "mmu.hpp"
#include "joypad.hpp"

const Uint32 kFramesPerSecond = 60;
const Uint32 kMillisPerFrame  = 1000 / kFramesPerSecond;

const tick_t kTicksPerFrame  = kTicksPerSecond / kFramesPerSecond;

uint8_t kInstrunctionLength[256] = {
    1, 3, 1, 1, 1, 1, 2, 1, 3, 1, 1, 1, 1, 1, 2, 1,    // 0x00 ~ 0x0F
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,    // 0x10 ~ 0x1F
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,    // 0x20 ~ 0x2F
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,    // 0x30 ~ 0x3F

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    // 0x40 ~ 0x4F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    // 0x50 ~ 0x5F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    // 0x60 ~ 0x6F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    // 0x70 ~ 0x7F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    // 0x80 ~ 0x8F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    // 0x90 ~ 0x9F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    // 0xA0 ~ 0xAF
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    // 0xB0 ~ 0xBF

    1, 1, 3, 3, 3, 1, 2, 1, 1, 1, 3, 1, 3, 3, 2, 1,    // 0xC0 ~ 0xCF
    1, 1, 3, 1, 3, 1, 2, 1, 1, 1, 3, 1, 3, 1, 2, 1,    // 0xD0 ~ 0xDF
    2, 1, 2, 1, 1, 1, 2, 1, 2, 1, 3, 1, 1, 1, 2, 1,    // 0xE0 ~ 0xEF
    2, 1, 2, 1, 1, 1, 2, 1, 2, 1, 3, 1, 1, 1, 2, 1     // 0xF0 ~ 0xFF
};

const char* kGBCPUInstrunctionNames[256] = {
    // 0x00 ~ 0x0F
    "nop",
    "ld bc,$%02x%02x",
    "ld (bc), a",
    "inc bc",
    "inc b",
    "dec b",
    "ld b,$%02x",
    "rlca",
    "add ($%02x%02x),sp",
    "add hl,bc",
    "ld a,(bc)",
    "dec bc",
    "inc c",
    "dec c",
    "ld c,$%02x",
    "rrca",

    // 0x10 ~ 0x1F
    "stop",
    "ld de,$%02x%02x",
    "ld (de),a",
    "inc de",
    "inc d",
    "dec d",
    "ld d,$%02x",
    "rla",
    "jr %hhd",
    "add hl,de",
    "ld a,(de)",
    "dec de",
    "inc e",
    "dec e",
    "ld e,$%02x",
    "rra",

    // 0x20 ~ 0x2F
    "jr nz %hhd",
    "ld hl,$%02x%02x",
    "ldi (hl),a",
    "inc hl",
    "inc h",
    "dec h",
    "ld h,$%02x",
    "daa",
    "jr z %hhd",
    "add hl,hl",
    "ldi a,(hl)",
    "dec hl",
    "inc l",
    "dec l",
    "ld l,$%02x",
    "cpl",

    // 0x30 ~ 0x3F
    "jr nc %hhd",
    "ld sp,$%02x%02x",
    "ldd (hl),a",
    "inc sp",
    "inc (hl)",
    "dec (hl)",
    "ld (hl),$%02x",
    "scf",
    "jr c %hhd",
    "add hl,sp",
    "ldd a,(hl)",
    "dec sp",
    "inc a",
    "dec a",
    "ld a,$%02x",
    "ccf",

    // 0x40 ~ 0x4F
    "ld b,b",
    "ld b,c",
    "ld b,d",
    "ld b,e",
    "ld b,h",
    "ld b,l",
    "ld b,(hl)",
    "ld b,a",
    "ld c,b",
    "ld c,c",
    "ld c,d",
    "ld c,e",
    "ld c,h",
    "ld c,l",
    "ld c,(hl)",
    "ld c,a",

    // 0x50 ~ 0x5F
    "ld d,b",
    "ld d,c",
    "ld d,d",
    "ld d,e",
    "ld d,h",
    "ld d,l",
    "ld d,(hl)",
    "ld d,a",
    "ld e,b",
    "ld e,c",
    "ld e,d",
    "ld e,e",
    "ld e,h",
    "ld e,l",
    "ld e,(hl)",
    "ld e,a",

    // 0x60 ~ 0x6F
    "ld h,b",
    "ld h,c",
    "ld h,d",
    "ld h,e",
    "ld h,h",
    "ld h,l",
    "ld h,(hl)",
    "ld h,a",
    "ld l,b",
    "ld l,c",
    "ld l,d",
    "ld l,e",
    "ld l,h",
    "ld l,l",
    "ld l,(hl)",
    "ld l,a",

    // 0x70 ~ 0x7F
    "ld (hl),b",
    "ld (hl),c",
    "ld (hl),d",
    "ld (hl),e",
    "ld (hl),h",
    "ld (hl),l",
    "ld (hl),(hl)",
    "ld (hl),a",
    "ld a,b",
    "ld a,c",
    "ld a,d",
    "ld a,e",
    "ld a,h",
    "ld a,l",
    "ld a,(hl)",
    "ld a,a",

    // 0x80 ~ 0x8F
    "add a,b",
    "add a,c",
    "add a,d",
    "add a,e",
    "add a,h",
    "add a,l",
    "add a,(hl)",
    "add a,a",
    "adc a,b",
    "adc a,c",
    "adc a,d",
    "adc a,e",
    "adc a,h",
    "adc a,l",
    "adc a,(hl)",
    "adc a,a",

    // 0x90 ~ 0x9F
    "sub a,b",
    "sub a,c",
    "sub a,d",
    "sub a,e",
    "sub a,h",
    "sub a,l",
    "sub a,(hl)",
    "sub a,a",
    "sbc a,b",
    "sbc a,c",
    "sbc a,d",
    "sbc a,e",
    "sbc a,h",
    "sbc a,l",
    "sbc a,(hl)",
    "sbc a,a",

    // 0xA0 ~ 0xAF
    "and a,b",
    "and a,c",
    "and a,d",
    "and a,e",
    "and a,h",
    "and a,l",
    "and a,(hl)",
    "and a,a",
    "xor a,b",
    "xor a,c",
    "xor a,d",
    "xor a,e",
    "xor a,h",
    "xor a,l",
    "xor a,(hl)",
    "xor a,a",

    // 0xB0 ~ 0xBF
    "or a,b",
    "or a,c",
    "or a,d",
    "or a,e",
    "or a,h",
    "or a,l",
    "or a,(hl)",
    "or a,a",
    "cp a,b",
    "cp a,c",
    "cp a,d",
    "cp a,e",
    "cp a,h",
    "cp a,l",
    "cp a,(hl)",
    "cp a,a",

    // 0xC0 ~ 0xCF
    "ret nz",
    "pop bc",
    "jp nz $%02x%02x",
    "jp $%02x%02x",
    "call nz $%02x%02x",
    "push bc",
    "add a,$%02x",
    "rst 00",
    "ret z",
    "ret",
    "jp z $%02x%02x",
    "[CB]",
    "call z $%02x%02x",
    "call $%02x%02x",
    "adc a,$%02x",
    "rst 08",

    // 0xD0 ~ 0xDF
    "ret nc",
    "pop de",
    "jp nc $%02x%02x",
    "[D3] - INVALID OPCODE",
    "call nc $%02x%02x",
    "push de",
    "sub $%02x",
    "rst 10",
    "ret c",
    "reti",
    "jp c $%02x%02x",
    "[DB] - INVALID OPCODE",
    "call c $%02x%02x",
    "[DD] - INVALID OPCODE",
    "sbc a,$%02x",
    "rst 18",

    // 0xE0 ~ 0xEF
    "ldh ($ff%02x),a",
    "pop hl",
    "ld (c),a",
    "[E3] - INVALID OPCODE",
    "[E4] - INVALID OPCODE",
    "push hl",
    "and $%02x",
    "rst 20",
    "add sp,$%02x",
    "jp hl",
    "ld ($%02x%02x),a",
    "[EB] - INVALID OPCODE",
    "[EC] - INVALID OPCODE",
    "[ED] - INVALID OPCODE",
    "xor $%02x",
    "rst 28",

    // 0xF0 ~ 0xFF
    "ldh a,($ff%02x)",
    "pop af",
    "ld a,(c)",
    "di",
    "[F4] - INVALID OPCODE",
    "push af",
    "or $%02x",
    "rst 30",
    "ld hl,sp+$%02x",
    "ld sp,hl",
    "ld a,(%02x%02x)",
    "ei",
    "[FC] - INVALID OPCODE",
    "[FD] - INVALID OPCODE",
    "cp $%02x",
    "rst 38"
};
