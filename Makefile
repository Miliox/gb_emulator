SOURCE = src/cpu.cpp src/clock.cpp src/mmu.cpp

.PHONY: test

all:
	@g++ -std=c++11 -Iinclude/ -o emulator $(SOURCE) src/main.cpp

test:
	@g++ -std=c++11 -Wall -Iinclude/ -Itest/ -o unit_tests $(SOURCE) test/*.cpp

clean:
	@rm -f emulator unit_tests
