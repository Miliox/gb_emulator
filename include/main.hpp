#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <string>

#include "cpu.hpp"
#include "gpu.hpp"
#include "mmu.hpp"
#include "joypad.hpp"

#include "debugger.hpp"

#include "instruction.hpp"

const Uint32 kFramesPerSecond = 60;
const Uint32 kMillisPerFrame  = 1000 / kFramesPerSecond;

const tick_t kTicksPerFrame  = kTicksPerSecond / kFramesPerSecond;

