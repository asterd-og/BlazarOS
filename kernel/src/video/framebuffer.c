#define SSFN_IMPLEMENTATION
#include <video/framebuffer.h>
#include <mm/heap/heap.h>

void fb_set_font(framebuffer_info* fb, int family, int size, u8* font) {
    fb->ssfn_buf.ptr = fb->buffer;
    fb->ssfn_buf.w = fb->width;
    fb->ssfn_buf.h = fb->height;
    fb->ssfn_buf.p = fb->pitch;
    fb->ssfn_buf.x = 0;
    fb->ssfn_buf.y = 0;
    fb->ssfn_buf.fg = 0xFFFFFFFF;
    ssfn_load(&fb->ssfn_ctx, font);
    int ret = ssfn_select(&fb->ssfn_ctx, family, NULL, SSFN_STYLE_REGULAR, size);
    if (ret != SSFN_OK) {
        serial_printf("%s\n", ssfn_error(ret));
    }
}

void fb_draw_str(framebuffer_info* fb, u32 x, u32 y, u32 color, char* str) {
    fb->ssfn_buf.x = x;
    fb->ssfn_buf.y = y;
    fb->ssfn_buf.fg = color;
    char* s = str;
    int ret = 0;
    while((ret = ssfn_render(&fb->ssfn_ctx, &fb->ssfn_buf, s)) > 0) s += ret;
}

void fb_set_pixel(framebuffer_info* fb, u32 x, u32 y, u32 color) {
    fb->buffer[y * fb->pitch / 4 + x] = color;
}

void fb_draw_tga(framebuffer_info* fb, u32 _x, u32 _y, u32* tga) {
    u32 w = tga[0];
    u32 h = tga[1];
    for (u32 y = 0; y < h; y++) {
        for (u32 x = 0; x < w; x++) {
            fb_set_pixel(fb, x + _x, y + _y, tga[2 + (x + (y * w))]);
        }
    }
}

void fb_clear(framebuffer_info* fb, u32 color) {
    memset(fb->buffer, color, fb->width * fb->height * 4);
}

framebuffer_info* fb_create(u32* buffer, u32 width, u32 height, u32 pitch) {
    framebuffer_info* fb = (framebuffer_info*)kmalloc(sizeof(framebuffer_info));
    memset(fb, 0, sizeof(framebuffer_info));
    fb->buffer = buffer;
    fb->width = width;
    fb->height = height;
    fb->pitch = pitch;

    memset(&fb->ssfn_ctx, 0, sizeof(ssfn_t));
    memset(&fb->ssfn_buf, 0, sizeof(ssfn_buf_t));

    return fb;
}