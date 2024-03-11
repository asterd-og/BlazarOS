#pragma once

#include <video/framebuffer.h>
#include "syscall.h"

bool bit_from_byte(u32 to_convert, int to_return) {
    int mask = 1 << (to_return - 1);
    return ((to_convert & mask) != 0);
}

font_info* font_get(u8 idx) {
    syscall(7, idx, 0, 0, 0, 0);
    font_info* rax = 0;
    asm volatile("" : "=a"(rax) : : "memory");
    return rax;
}

void fb_set_pixel(framebuffer_info* fb, u32 x, u32 y, u32 color) {
    if (x > fb->width || x < 0 || y > fb->height || y < 0) return;
    fb->buffer[y * fb->pitch / 4 + x] = color;
}

void fb_draw_char(framebuffer_info* fb, u32 x, u32 y, u32 color, char c, font_info font) {
    if (c == 0) return;

    u32 p = font.height * c;
    
    for (u32 xx = 0; xx < font.width; xx++) {
        for (u32 yy = 0; yy < font.height; yy++) {
            if (bit_from_byte(font.data[p + yy], xx + 1))
                fb_set_pixel(fb, x + (font.width - xx), y + yy, color);
        }
    }
}

void fb_draw_str(framebuffer_info* fb, u32 x, u32 y, u32 color, char* c, font_info font) {
    while (*c) {
        fb_draw_char(fb, x, y, color, *c, font);
        c++;
        x += font.width;
    }
}