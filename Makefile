all:
	@g++ -std=c++11 -Iinclude/ -o emulator src/cpu.cpp src/main.cpp

clean:
	@rm emulator
