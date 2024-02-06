#pragma once

#include <types.h>
#include <limine.h>
#include <video/ssfn.h>

typedef struct {
    u32* buffer;
    u32 width;
    u32 height;
    u32 pitch;
    ssfn_t ssfn_ctx;
    ssfn_buf_t ssfn_buf;
} __attribute__((packed)) framebuffer_info;

framebuffer_info* fb_create(u32* buffer, u32 width, u32 height, u32 pitch);

void fb_set_font(framebuffer_info* fb, int family, int size, u8* font);
void fb_draw_str(framebuffer_info* fb, u32 x, u32 y, u32 color, char* c);

void fb_set_pixel(framebuffer_info* fb, u32 x, u32 y, u32 color);

void fb_draw_tga(framebuffer_info* fb, u32 x, u32 y, u32* tga);

void fb_clear(framebuffer_info* fb, u32 color);

extern framebuffer_info* vbe;