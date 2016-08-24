all:
	g++ -std=c++11 -Isrc/ -o emulator src/cpu.cpp src/main.cpp

