SOURCE = src/cpu.cpp src/mmu.cpp src/gpu.cpp
CFLAGS = -std=c++11 -Wall `(sdl2-config --cflags)` -Iinclude/ `(sdl2-config --libs)`

.PHONY: test

all:
	@g++ $(CFLAGS) -o emulator $(SOURCE) src/main.cpp

test:
	@g++ $(CFLAGS) -Itest/ -o unit_tests $(SOURCE) test/*.cpp

clean:
	@rm -f emulator unit_tests
