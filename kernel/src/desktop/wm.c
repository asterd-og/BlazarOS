#include <desktop/wm.h>
#include <mm/heap/heap.h>
#include <dev/ps2/mouse.h>
#include <desktop/window.h>
#include <video/framebuffer.h>

wm_theme_info* wm_theme = NULL;

window_info* wm_window_list[128];
u64 wm_window_list_idx = 0;

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

u32* wm_fb_buffer = NULL;
framebuffer_info* wm_fb = NULL;

window_info* wm_create_window(int x, int y, int width, int height, char* title) {
    window_info* win = window_create(x, y, width, height, title);
    wm_window_list[wm_window_list_idx] = win;
    wm_window_list_idx++;
    return win;
}

void wm_draw_mouse() {
    for (u32 y = 0; y < 19; y++) {
        for (u32 x = 0; x < 11; x++) {
            fb_set_pixel(vbe, wm_mouse_rect.x + x, wm_mouse_rect.y + y, fb_get_pixel(wm_fb, wm_mouse_rect.x + x, wm_mouse_rect.y + y));
        }
    }

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
    for (int i = 0; i < wm_window_list_idx; i++) {
        if (wm_window_list[i]->dirty) {
            window_info* win = wm_window_list[i];
            fb_clear(wm_fb, 0xFFFF00FF);
            window_draw_decorations(win);
            fb_blit_fb(wm_fb, win->fb, win->rect.x, win->rect.y);
            fb_blit_fb(vbe, wm_fb, 0, 0);
            wm_draw_mouse();
            vbe_swap();
            wm_window_list[i]->dirty = false;
        }
    }
    if (mouse_moved) {
        wm_draw_mouse();
        vbe_swap();
        mouse_moved = false;
    }
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

    wm_fb_buffer = (u32*)kmalloc(vbe->height * vbe->width * 4);
    wm_fb = fb_create(wm_fb_buffer, vbe->width, vbe->height, vbe->pitch);
}