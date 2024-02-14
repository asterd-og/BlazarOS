#pragma once

#include <types.h>

typedef struct {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
} rectangle;

inline bool rect_colliding(rectangle a, rectangle b) {
    if (a.x >= b.x && a.x <= b.x + b.width &&
        a.y >= b.y && b.y <= b.y + b.height) return true;
    return false;
}

inline bool rect_colliding_coord(u32 ax, u32 ay, u32 bx, u32 by, u32 bw, u32 bh) {
    if (ax >= bx && ax <= bx + bw &&
        ay >= by && ay <= by + bh) {
            return true;
        }
    return false;
}