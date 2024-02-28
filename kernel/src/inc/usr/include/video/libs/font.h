#pragma once

#include <types.h>
#include <video/libs/fonts_data.h>

typedef struct
{
    u8  width;
    u8  height;
    u8  spacing_x;
    u8  spacing_y;
    const u8* data;
} __attribute__((packed)) font_info;

static const font_info FONT_SLIM_8x16   = { 8, 16, 0, 0, FONTDATA_SLIM_8x16 };

static inline bool bit_address_from_byte(u32 to_convert, int to_return)
{
    int mask = 1 << (to_return - 1);
    return ((to_convert & mask) != 0);
}
#define kernel_font FONT_SLIM_8x16