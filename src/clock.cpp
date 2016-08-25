#include "clock.hpp"

const Clock Clock::operator+(const Clock& rhs) {
    return Clock(this->m + rhs.m, this->t + rhs.t);
}

Clock& Clock::operator+=(const Clock& rhs) {
    this->m += rhs.m;
    this->t += rhs.t;
    return *this;
}
