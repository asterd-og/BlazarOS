#include <desktop/theme/theme_man.h>
#include <video/framebuffer.h>
#include <dev/initrd/blazfs.h>
#include <mm/heap/heap.h>
#include <desktop/rect.h>

tga_info* theme_tga;
theme_info* theme_global;

void theme_crop_rect_into(u32 x, u32 y, u32 w, u32 h, u32 to_w, u32* from, u32* to) {
    for (u32 xx = 0; xx < w; xx++) {
        for (u32 yy = 0; yy < h; yy++) {
            to[xx + (yy * w)] = from[(xx + x) + ((yy + y) * to_w)];
        }
    }
}

void theme_init(char* theme_dir) {
    u32 size = blazfs_ftell(theme_dir);

    u8* theme_buf = (u8*)kmalloc(size);
    blazfs_read(theme_dir, theme_buf);

    theme_tga = tga_parse(theme_buf, size);

    theme_global = (theme_info*)kmalloc(sizeof(theme_info));

    theme_global->image = theme_tga;

    // Button
    // Idea: Begin parsing .ini files to get elements dimensions, etc.
    theme_global->button_info.start_x = 0;
    theme_global->button_info.start_y = 0;

    theme_global->button_info.ls_width = 4;
    theme_global->button_info.ls_height = 17;

    theme_global->button_info.ms_width = 8;
    theme_global->button_info.ms_height = 17;

    theme_global->button_info.rs_width = 4;
    theme_global->button_info.rs_height = 17;

    // Window
    theme_global->window_info.start_x = 20;
    theme_global->window_info.start_y = 0;

    theme_global->window_info.bar_ls_width = 5;
    theme_global->window_info.bar_ls_height = 17;

    theme_global->window_info.bar_ms_width = 1;
    theme_global->window_info.bar_ms_height = 17;
    
    theme_global->window_info.bar_rs_width = 5;
    theme_global->window_info.bar_rs_height = 17;


    theme_global->window_info.ls_width = 5;
    theme_global->window_info.ls_height = 1;

    theme_global->window_info.rs_width = 4;
    theme_global->window_info.rs_height = 1;


    theme_global->window_info.bottom_ls_height = 4;
    theme_global->window_info.bottom_ls_width = 5;

    theme_global->window_info.bottom_ms_height = 4;
    theme_global->window_info.bottom_ms_width = 1;

    theme_global->window_info.bottom_rs_height = 4;
    theme_global->window_info.bottom_rs_width = 4;
}

void theme_test() {
}