#include <desktop/theme/theme_man.h>
#include <video/framebuffer.h>
#include <dev/initrd/blazfs.h>
#include <mm/heap/heap.h>
#include <desktop/rect.h>

tga_info* theme_tga;

u32* btn_part1;
u32* btn_part2;
u32* btn_part3;

void theme_crop_rect_into(u32 x, u32 y, u32 w, u32 h, u32 to_w, u32* from, u32* to) {
    for (u32 xx = 0; xx < w; xx++) {
        for (u32 yy = 0; yy < h; yy++) {
            to[xx + (yy * w)] = from[(xx + x) + ((yy + y) * to_w)];
        }
    }
}

void theme_get_button() {
    u32 size = blazfs_ftell("theme.tga");

    u8* theme_buf = (u8*)kmalloc(size);
    blazfs_read("theme.tga", theme_buf);

    theme_tga = tga_parse(theme_buf, size);

    btn_part1 = (u32*)kmalloc(5 * 17 * 4);
    btn_part2 = (u32*)kmalloc(9 * 17 * 4);
    btn_part3 = (u32*)kmalloc(5 * 17 * 4);

    theme_crop_rect_into(0, 0, 5, 17, theme_tga->width, theme_tga->data, btn_part1);
    theme_crop_rect_into(6, 0, 9, 17, theme_tga->width, theme_tga->data, btn_part2);
    theme_crop_rect_into(6 + 9 + 1, 0, 5, 17, theme_tga->width, theme_tga->data, btn_part3);
}

void theme_render_button() {
    fb_draw_buffer(vbe, 0, 0, 5, 17, btn_part1);
    fb_draw_buffer(vbe, 4, 0, 9, 17, btn_part2);
    fb_draw_buffer(vbe, 4 + 9, 0, 9, 17, btn_part2);
    fb_draw_buffer(vbe, 4 + 18, 0, 9, 17, btn_part2);
    fb_draw_buffer(vbe, 4 + 27, 0, 9, 17, btn_part2);
    fb_draw_buffer(vbe, 4 + 36, 0, 9, 17, btn_part2);
    fb_draw_buffer(vbe, 4 + 45, 0, 9, 17, btn_part2);
    fb_draw_buffer(vbe, 4 + 54, 0, 9, 17, btn_part2);
    fb_draw_buffer(vbe, 4 + 54 + 9, 0, 5, 17, btn_part3);

    fb_draw_str(vbe, 4, 2, 0xFF000000, "Buttons!", kernel_font);
}