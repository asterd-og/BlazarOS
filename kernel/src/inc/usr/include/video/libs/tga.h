#pragma once

#include <types.h>

typedef struct {
    u32 width;
    u32 height;
    u32* data;
} tga_info;

tga_info* tga_parse(u8* ptr, u32 size);