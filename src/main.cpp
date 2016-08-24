#include <iostream>
#include <cpu.hpp>

using namespace std;

int main(int argc, char** argv) {
	Z80 cpu;
	cpu.reset();

	cout << "Z80.reg.a: " << cpu.reg.a << endl;
	cout << "Z80.reg.b: " << cpu.reg.b << endl;
	cout << "Z80.reg.c: " << cpu.reg.c << endl;
	cout << "Z80.reg.d: " << cpu.reg.d << endl;
	cout << "Z80.reg.e: " << cpu.reg.e << endl;
	cout << "Z80.reg.f: " << cpu.reg.f << endl;
	cout << "Z80.reg.h: " << cpu.reg.h << endl;
	cout << "Z80.reg.l: " << cpu.reg.l << endl;
	cout << "Z80.reg.sp: " << cpu.reg.sp << endl;
	cout << "Z80.reg.pc: " << cpu.reg.pc << endl;
	cout << "Z80.clock.m: " << cpu.clock.m << endl;
	cout << "Z80.clock.t: " << cpu.clock.t << endl;

	return 0;
}
