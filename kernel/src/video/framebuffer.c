#define SSFN_IMPLEMENTATION
#include <video/framebuffer.h>
#include <mm/heap/heap.h>

void fb_set_font(framebuffer_info* fb, int family, int size, u8* font) {
    fb->ssfn_buf.ptr = fb->buffer;
    fb->ssfn_buf.w = fb->width;
    fb->ssfn_buf.h = fb->height;
    fb->ssfn_buf.p = fb->pitch;
    fb->ssfn_buf.x = 500;
    fb->ssfn_buf.y = 500;
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
    while (*str) {
        char tmp[] = {*str, 0};
        ssfn_render(&fb->ssfn_ctx, &fb->ssfn_buf, tmp);
        *str++;
    }
}

void fb_set_pixel(framebuffer_info* fb, u32 x, u32 y, u32 color) {
    fb->buffer[y * fb->pitch + x] = color;
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