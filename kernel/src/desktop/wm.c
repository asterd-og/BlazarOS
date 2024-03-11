#include <desktop/wm.h>
#include <mm/heap/heap.h>
#include <dev/ps2/mouse.h>
#include <dev/ps2/keyboard.h>
#include <desktop/window.h>
#include <video/framebuffer.h>
#include <video/vbe.h>
#include <video/libs/tga.h>
#include <dev/initrd/blazfs.h>

wm_theme_info* wm_theme = NULL;

window_info* wm_window_list[128];
u8 wm_window_list_idx = 0;

u8 wm_window_z_order[128];
u8 wm_window_z_idx = 0;

rectangle wm_mouse_rect = {.x = 0, .y = 0, .width = 1, .height = 1};

u8 wm_cursor[4][13][11] = {
    // Normal cursor
    {
        {1,1,0,0,0,0,0,0,0,0,0},
        {1,2,1,0,0,0,0,0,0,0,0},
        {1,2,2,1,0,0,0,0,0,0,0},
        {1,2,2,2,1,0,0,0,0,0,0},
        {1,2,2,2,2,1,0,0,0,0,0},
        {1,2,2,2,2,2,1,0,0,0,0},
        {1,2,2,2,2,2,1,0,0,0,0},
        {1,1,1,2,1,1,1,0,0,0,0},
        {0,0,1,2,2,1,0,0,0,0,0},
        {0,0,0,1,2,2,1,0,0,0,0},
        {0,0,0,0,1,2,2,1,0,0,0},
        {0,0,0,0,0,1,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0},
    },
    {
        {0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,1,1,1,1,1,0,0,0},
        {0,0,1,2,1,2,1,2,1,0,0},
        {0,1,1,2,1,2,1,2,2,1,0},
        {1,2,1,2,2,2,2,2,2,2,1},
        {1,2,1,2,2,2,2,2,2,2,1},
        {0,1,2,2,2,2,2,2,2,1,0},
        {0,1,2,2,2,2,2,2,2,1,0},
        {0,0,1,2,2,2,2,2,1,0,0},
        {0,0,0,1,1,1,1,1,0,0,0},
    },
    {
        {0,0,0,1,0,0,0,0,0,0,0},
        {0,0,1,2,1,0,0,0,0,0,0},
        {0,0,1,2,1,0,0,0,0,0,0},
        {0,0,1,2,1,0,0,0,0,0,0},
        {0,1,1,2,1,1,1,1,0,0,0},
        {1,2,1,2,1,2,1,2,1,0,0},
        {1,2,1,2,1,2,1,2,2,1,0},
        {1,2,1,2,2,2,2,2,2,2,1},
        {1,2,1,2,2,2,2,2,2,2,1},
        {0,1,2,2,2,2,2,2,2,1,0},
        {0,1,2,2,2,2,2,2,2,1,0},
        {0,0,1,2,2,2,2,2,1,0,0},
        {0,0,0,1,1,1,1,1,0,0,0},
    },
    {
        {0,0,0,1,1,1,1,1,0,0,0},
        {0,0,0,1,2,2,2,1,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,0,1,2,1,0,0,0,0},
        {0,0,0,1,2,2,2,1,0,0,0},
        {0,0,0,1,1,1,1,1,0,0,0},
    }
};

u32* wm_fb_buffer = NULL;
framebuffer_info* wm_fb = NULL;
tga_info* wpp = NULL;

bool wm_redraw = true;
bool wm_redrawn = false;
bool wm_moving_window = false;

u8 wm_cursor_state = 0;

window_info* wm_create_window(int x, int y, int width, int height, char* title) {
    window_info* win = window_create(x, y, width, height, title);
    win->win_idx = wm_window_list_idx;

    wm_window_list[wm_window_list_idx] = win;
    wm_window_z_order[wm_window_z_idx] = wm_window_list_idx;
    
    wm_window_list_idx++;
    wm_window_z_idx++;
    return win;
}

void wm_draw_mouse() {
    for (u32 y = 0; y < 13; y++) {
        for (u32 x = 0; x < 11; x++) {
            fb_set_pixel(vbe, wm_mouse_rect.x + x, wm_mouse_rect.y + y, fb_get_pixel(wm_fb, wm_mouse_rect.x + x, wm_mouse_rect.y + y));
        }
    }

    wm_mouse_rect.x = mouse_x;
    wm_mouse_rect.y = mouse_y;

    for (u32 y = 0; y < 13; y++) {
        for (u32 x = 0; x < 11; x++) {
            switch (wm_cursor[wm_cursor_state][y][x]) {
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
    if (wm_redraw) {
        if (wm_redrawn) {
            wm_redraw = false;
            wm_redrawn = false;
        } else {
            fb_draw_tga(wm_fb, 0, 0, wpp);
            wm_redrawn = true;
        }
    }

    for (int i = 0; i < wm_window_z_idx; i++) {
        window_info* win = wm_window_list[wm_window_z_order[i]];
        if (mouse_moved) {
            window_update(win);
        }
        if (i == wm_window_z_idx - 1) {
            for (int j = 0; j < win->element_count; j++) {
                win->elements[j]->update(win->elements[j]);
            }
            if (win->dirty || win->fb_dirty || wm_redraw) {
                if (win->dirty || wm_redraw) {
                    window_draw_decorations(win);
                }
                if (win->fb_dirty) {
                    for (int j = 0; j < win->element_count; j++) {
                        if (win->elements[j]->dirty) {
                            win->elements[j]->draw(win->elements[j]);
                            win->elements[j]->dirty = false;
                        }
                    }
                }
                fb_blit_fb(wm_fb, win->fb, win->rect.x, win->rect.y);
                fb_blit_fb(vbe, wm_fb, 0, 0);
                wm_draw_mouse();
                vbe_swap();
                win->dirty = false;
                win->fb_dirty = false;
            }
        } else {
            if (wm_redraw) {
                window_draw_decorations(win);
                fb_blit_fb(wm_fb, win->fb, win->rect.x, win->rect.y);
            }
        }
    }
    if (mouse_moved) {
        mouse_moved = false;
    }

    if (wm_redraw) wm_mouse_task();
}

void wm_mouse_task() {
    wm_draw_mouse();
    vbe_swap();
}

void wm_begin_draw(window_info* win) {
    // TODO: ?
}

void wm_end_draw(window_info* win) {
    win->fb_dirty = true;
}

void wm_bring_to_front(u8 win_idx) {
    for (u8 i = 0; i < wm_window_z_idx; i++) {
        if (wm_window_z_order[i] == win_idx) {
            if (i == wm_window_z_idx) break;
            memcpy(wm_window_z_order + i, wm_window_z_order + (i + 1), wm_window_z_idx - i);
            wm_window_z_order[wm_window_z_idx - 1] = win_idx;
            break;
        }
    }
}

char wm_get_key(window_info* win) {
    if (win->win_idx == wm_window_z_order[wm_window_z_idx - 1]) {
        return fifo_pop(keyboard_fifo);
    }
    return 0;
}

void wm_clear_key(window_info* win) {
    if (win->win_idx == wm_window_z_order[wm_window_z_idx - 1]) {
        fifo_clear(keyboard_fifo);
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

    u8* buf = kmalloc(blazfs_ftell("wpp.tga"));
    blazfs_read("wpp.tga", buf);
    wpp = tga_parse(buf, blazfs_ftell("wpp.tga"));

    fb_draw_tga(wm_fb, 0, 0, wpp);
}