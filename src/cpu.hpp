#include <cstdint>

class Clock {
public:
	uint8_t m;
	uint8_t t;
};

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
};

class Z80 {
public:
	Registers reg;
	Clock     clock;

	void reset();
};
