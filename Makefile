SOURCE = src/cpu.cpp src/mmu.cpp src/gpu.cpp src/cartridge.cpp src/mbc.cpp src/joypad.cpp src/debugger.cpp src/instruction.cpp src/utils.cpp
CFLAGS = -std=c++11 -Wall `(sdl2-config --cflags)` -Iinclude/ `(sdl2-config --libs)` -lSDL2_ttf

.PHONY: test

all:
	@g++ $(CFLAGS) -o emulator $(SOURCE) src/main.cpp

test:
	@g++ $(CFLAGS) -Itest/ -o unit_tests $(SOURCE) test/*.cpp

clean:
	@rm -f emulator unit_tests
