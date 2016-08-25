#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <cstdint>

class Clock {
public:
    uint8_t m; // machine cycles
    uint8_t t; // clock cycles

    Clock() : m(0), t(0) {}
    Clock(uint8_t m) : m(m), t(4 * m) {}
    Clock(uint8_t m, uint8_t t) : m(m), t(t) {}

    const Clock operator+(const Clock&);
    Clock& operator+=(const Clock&);
};

#endif
