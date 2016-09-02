#include "sprite.hpp"

GBSprite::GBSprite(uint8_t x, uint8_t y, uint8_t tile, uint8_t flags)
    : x(x), y(y), tile(tile),
    priority(flags & 0x80), y_flip(flags & 0x40), x_flip(flags & 0x20), is_pallete_1(flags & 0x10) {

}
