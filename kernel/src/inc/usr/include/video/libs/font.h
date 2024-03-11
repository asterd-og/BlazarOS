#pragma once

#include <types.h>

typedef struct
{
    u8  width;
    u8  height;
    u8  spacing_x;
    u8  spacing_y;
    u8* data;
} __attribute__((packed)) font_info;

static inline bool bit_address_from_byte(u32 to_convert, int to_return)
{
    int mask = 1 << (to_return - 1);
    return ((to_convert & mask) != 0);
}

void font_init();
font_info* font_get(u8 idx);

#define kernel_font *font_get(0)