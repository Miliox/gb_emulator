#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "mmu.hpp"

class GBSprite {
private:

public:
    GBSprite(uint8_t x, uint8_t y, uint8_t tile, uint8_t flags);

    uint8_t x;
    uint8_t y;
    uint8_t tile;

    bool priority;
    bool y_flip;
    bool x_flip;
    bool is_pallete_1;
};
#endif
