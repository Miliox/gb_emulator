all:
	@g++ -std=c++11 -Iinclude/ -o emulator src/*.cpp

clean:
	@rm emulator
