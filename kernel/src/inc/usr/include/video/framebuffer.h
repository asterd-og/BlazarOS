#pragma once

#include <types.h>
#include <limine.h>
#include <video/libs/tga.h>
#include <video/libs/font.h>

typedef struct {
    u32* buffer;
    u32 width;
    u32 height;
    u32 pitch;
} __attribute__((packed)) framebuffer_info;

framebuffer_info* fb_create(u32* buffer, u32 width, u32 height, u32 pitch);

void fb_draw_char(framebuffer_info* fb, u32 x, u32 y, u32 color, char c, font_info font);
void fb_draw_str(framebuffer_info* fb, u32 x, u32 y, u32 color, char* c, font_info font);

void fb_set_pixel(framebuffer_info* fb, u32 x, u32 y, u32 color);
u32 fb_get_pixel(framebuffer_info* fb, u32 x, u32 y);

void fb_draw_rectangle(framebuffer_info* fb, u32 x, u32 y, u32 w, u32 h, u32 color);
void fb_draw_outline(framebuffer_info* fb, u32 x, u32 y, u32 w, u32 h, u32 color);

void fb_draw_tga(framebuffer_info* fb, u32 x, u32 y, tga_info* tga);
void fb_draw_fake_alpha_tga(framebuffer_info* fb, u32 x, u32 y, u32 alpha_color, tga_info* tga);

void fb_draw_buffer(framebuffer_info* fb, u32 x, u32 y, u32 w, u32 h, u32* buf);
void fb_draw_fake_alpha_buffer(framebuffer_info* fb, u32 x, u32 y, u32 w, u32 h, u32 alpha, u32* buf);

void fb_blit_fb(framebuffer_info* to, framebuffer_info* from, u32 x, u32 y);

void fb_clear(framebuffer_info* fb, u32 color);

extern framebuffer_info* vbe;