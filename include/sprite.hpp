#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <cstdint>

#pragma pack(push, 1)
class GBSprite {
private:

public:
    GBSprite(uint8_t y = 0, uint8_t x = 0, uint8_t tile = 0, uint8_t flags = 0)
        : y(y), x(x), tile(tile), flags(flags) {}

    const uint8_t y;
    const uint8_t x;
    const uint8_t tile;
    const uint8_t flags;

    bool has_priority() { return flags & 0x80; }
    bool is_yflipped() { return flags & 0x40; }
    bool is_xflipped() { return flags & 0x20; }
    bool is_pallet1() { return flags & 0x10; }

    uint8_t screenX() { return x - 8; }
    uint8_t screenY() { return y - 16; }

};
#pragma pack(pop)
#endif
