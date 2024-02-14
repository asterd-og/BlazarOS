#include <desktop/wm.h>
#include <mm/heap/heap.h>
#include <dev/ps2/mouse.h>

wm_theme_info* wm_theme;

rectangle wm_mouse_rect;

u32 wm_cursor_data[19][11] = {
    {1,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0,0},
    {1,2,2,2,2,2,2,1,0,0,0},
    {1,2,2,2,2,2,2,2,1,0,0},
    {1,2,2,2,2,2,2,2,2,1,0},
    {1,2,2,2,2,2,1,1,1,1,1},
    {1,2,2,1,2,2,1,0,0,0,0},
    {1,2,1,0,1,2,2,1,0,0,0},
    {1,1,0,0,1,2,2,1,0,0,0},
    {1,0,0,0,0,1,2,2,1,0,0},
    {0,0,0,0,0,1,2,2,1,0,0},
    {0,0,0,0,0,0,1,2,2,1,0},
    {0,0,0,0,0,0,1,2,2,1,0},
    {0,0,0,0,0,0,0,1,1,0,0},
};

void wm_draw_mouse() {
    wm_mouse_rect.x = mouse_x;
    wm_mouse_rect.y = mouse_y;

    for (u32 y = 0; y < 19; y++) {
        for (u32 x = 0; x < 11; x++) {
            switch (wm_cursor_data[y][x]) {
                case 1:
                    fb_set_pixel(vbe, x + mouse_x, y + mouse_y, wm_theme->mouse_outline);
                    break;
                case 2:
                    fb_set_pixel(vbe, x + mouse_x, y + mouse_y, wm_theme->mouse_inline);
                    break;
                default:
                    break;
            }
        }
    }
}

void wm_update() {
    wm_draw_mouse();
}

void wm_init() {
    wm_theme = (wm_theme_info*)kmalloc(sizeof(wm_theme_info));
    wm_theme->window_title = 0xFF722f37;
    wm_theme->window_title_font = 0xFFFFFFFF;

    wm_theme->mouse_inline = 0xFF000000;
    wm_theme->mouse_outline = 0xFFFFFFFF;

    wm_mouse_rect.x = 0;
    wm_mouse_rect.y = 0;
    wm_mouse_rect.width = 1;
    wm_mouse_rect.height = 1;
}