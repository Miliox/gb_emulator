#include <cpu.hpp>
#include <cstring>

void Z80::reset() {
	std::memset(this, 0, sizeof(Z80));
}
