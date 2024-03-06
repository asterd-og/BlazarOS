#pragma once

#include <types.h>

typedef struct {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
} rectangle;

bool rect_colliding(rectangle a, rectangle b);
bool rect_colliding_coord(u32 ax, u32 ay, u32 bx, u32 by, u32 bw, u32 bh); 