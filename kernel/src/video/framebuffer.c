#include <video/framebuffer.h>
#include <mm/heap/heap.h>

void fb_draw_char(framebuffer_info* fb, u32 x, u32 y, u32 color, char c, font_info font) {
    if (c == 0) return;

    u32 p = font.height * c;
    
    for (u32 xx = 0; xx < font.width; xx++) {
        for (u32 yy = 0; yy < font.height; yy++) {
            if (bit_address_from_byte(font.data[p + yy], xx + 1))
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

void fb_set_pixel(framebuffer_info* fb, u32 x, u32 y, u32 color) {
    if (x > fb->width || x < 0 || y > fb->height || y < 0) return;
    fb->buffer[y * fb->pitch / 4 + x] = color;
}

u32 fb_get_pixel(framebuffer_info* fb, u32 x, u32 y) {
    return fb->buffer[y * fb->width + x];
}

void fb_draw_rectangle(framebuffer_info* fb, u32 x, u32 y, u32 w, u32 h, u32 color) {
    for (u32 xx = x; xx < w + x; xx++) {
        for (u32 yy = y; yy < y + h; yy++) {
            fb->buffer[yy * fb->pitch / 4 + xx] = color;
        }
    }
}

void fb_draw_outline(framebuffer_info* fb, u32 x, u32 y, u32 w, u32 h, u32 color) {
    for (u32 xx = x; xx < w + x; xx++) {
        fb->buffer[y * fb->pitch / 4 + xx] = color;
    }

    for (u32 yy = y; yy < y + h; yy++) {
        fb->buffer[yy * fb->pitch / 4 + (x + w)] = color;
        fb->buffer[yy * fb->pitch / 4 + x] = color;
    }

    for (u32 xx = x; xx < w + x + 1; xx++) {
        fb->buffer[(y + h) * fb->pitch / 4 + xx] = color;
    }
}

void fb_draw_tga(framebuffer_info* fb, u32 x, u32 y, tga_info* tga) {
    for (u32 yy = 0; yy < tga->height; yy++) {
        for (u32 xx = 0; xx < tga->width; xx++) {
            fb_set_pixel(fb, xx + x, yy + y, tga->data[xx + (yy * tga->width)]);
        }
    }
}

void fb_draw_fake_alpha_tga(framebuffer_info* fb, u32 x, u32 y, u32 alpha, tga_info* tga) {
    for (u32 yy = 0; yy < tga->height; yy++) {
        for (u32 xx = 0; xx < tga->width; xx++) {
            u32 pixel = tga->data[xx + (yy * tga->width)];
            if (pixel != alpha) fb_set_pixel(fb, xx + x, yy + y, pixel);
        }
    }
}

void fb_draw_buffer(framebuffer_info* fb, u32 x, u32 y, u32 w, u32 h, u32* buf) {
    for (u32 yy = 0; yy < h; yy++) {
        for (u32 xx = 0; xx < w; xx++) {
            fb_set_pixel(fb, xx + x, yy + y, buf[xx + (yy * w)]);
        }
    }
}

void fb_draw_fake_alpha_buffer(framebuffer_info* fb, u32 x, u32 y, u32 w, u32 h, u32 alpha, u32* buf) {
    for (u32 yy = 0; yy < h; yy++) {
        for (u32 xx = 0; xx < w; xx++) {
            u32 pixel = buf[xx + (yy * w)];
            if (pixel != alpha) fb_set_pixel(fb, xx + x, yy + y, pixel);
        }
    }
}

void fb_blit_fb(framebuffer_info* to, framebuffer_info* from, u32 x, u32 y) {
    for (u32 xx = 0; xx < from->width; xx++) {
        for (u32 yy = 0; yy < from->height; yy++) {
            fb_set_pixel(to, xx + x, yy + y, fb_get_pixel(from, xx, yy));
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

    return fb;
}