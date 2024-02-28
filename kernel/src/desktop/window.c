#include <desktop/window.h>
#include <mm/heap/heap.h>
#include <desktop/theme/theme_man.h>

u32* win_bar_ls;
u32* win_bar_ms;
u32* win_bar_rs;

u32* win_ls;
u32* win_rs;

u32* win_bottom_ls;
u32* win_bottom_ms;
u32* win_bottom_rs;

void window_draw_decorations(window_info* win) {
    theme_window_info* inf = &theme_global->window_info;

    fb_draw_fake_alpha_buffer(wm_fb, (win->rect.x - inf->bar_ls_width), win->rect.y - inf->bar_ls_height, inf->bar_ls_width, inf->bar_ls_height, 0xFF00FF00, win_bar_ls);
    fb_draw_fake_alpha_buffer(wm_fb, ((win->rect.x - inf->bar_ls_width) + (inf->bar_ls_width + win->rect.width)), win->rect.y - inf->bar_ls_height, inf->bar_rs_width, inf->bar_rs_height, 0xFF00FF00, win_bar_rs);
    
    fb_draw_buffer(wm_fb, (win->rect.x - inf->bar_ls_width), win->rect.y + win->rect.height, inf->bottom_ls_width, inf->bottom_ls_height, win_bottom_ls);
    fb_draw_buffer(wm_fb, ((win->rect.x - inf->bar_ls_width) + (inf->bar_ls_width + win->rect.width)), win->rect.y + win->rect.height, inf->bottom_rs_width, inf->bottom_rs_height, win_bottom_rs);

    for (u32 i = 0; i < win->rect.width; i++) {
        fb_draw_buffer(wm_fb, ((win->rect.x - inf->bar_ls_width) + (inf->bar_ls_width + i)), win->rect.y - inf->bar_ls_height, inf->bar_ms_width, inf->bar_ms_height, win_bar_ms);
        fb_draw_buffer(wm_fb, ((win->rect.x - inf->bar_ls_width) + (inf->bottom_ls_width + i)), win->rect.y + win->rect.height, inf->bottom_ms_width, inf->bottom_ms_height, win_bottom_ms);
    }

    for (u32 i = 0; i < win->rect.height; i++) {
        fb_draw_buffer(wm_fb, (win->rect.x - inf->bar_ls_width), win->rect.y + i, inf->ls_width, inf->ls_height, win_ls);
        fb_draw_buffer(wm_fb, (win->rect.x - inf->bar_ls_width) + win->rect.width + inf->bar_rs_width, win->rect.y + i, inf->rs_width, inf->rs_height, win_rs);
    }

    fb_draw_str(wm_fb, win->rect.x + inf->bar_ls_width, (win->rect.y - inf->bar_ls_height) + 2, 0xFF000000, win->title, kernel_font);
}

void window_add_element(window_info* win, element_info* element) {
    win->elements[win->element_count++] = (struct element_info*)element;
}

window_info* window_create(u32 x, u32 y, u32 width, u32 height, char* name) {
    window_info* win = (window_info*)kmalloc(sizeof(window_info));
    win->title = (char*)kmalloc(strlen(name));
    win->title_len = strlen(name);
    memcpy(win->title, name, strlen(name));

    win->rect.x = x;
    win->rect.y = y;
    win->rect.width = width;
    win->rect.height = height;

    win->dirty = true;
    win->fb_dirty = true;

    win->buffer = (u32*)kmalloc(width * height * 4);
    win->pitch = width * 4;

    win->elements = (struct element_info**)kmalloc(sizeof(element_info) * 256);
    memset(win->elements, 0, sizeof(element_info) * 256);
    win->element_count = 0;

    win->fb = fb_create(win->buffer, width, height, win->pitch);
    fb_clear(win->fb, 0xff800080);

    return win;
}

void window_init() {
    tga_info* tga = theme_global->image;
    theme_window_info win = theme_global->window_info;

    win_bar_ls = (u32*)kmalloc(win.bar_ls_width * win.bar_ls_height * 4);
    win_bar_ms = (u32*)kmalloc(win.bar_ms_width * win.bar_ms_height * 4);
    win_bar_rs = (u32*)kmalloc(win.bar_rs_width * win.bar_rs_height * 4);

    win_ls = (u32*)kmalloc(win.ls_width * win.ls_height * 4);
    win_rs = (u32*)kmalloc(win.rs_width * win.rs_height * 4);

    win_bottom_ls = (u32*)kmalloc(win.bottom_ls_width * win.bottom_ls_height * 4);
    win_bottom_ms = (u32*)kmalloc(win.bottom_ms_width * win.bottom_ms_height * 4);
    win_bottom_rs = (u32*)kmalloc(win.bottom_rs_width * win.bottom_rs_height * 4);

    theme_crop_rect_into(win.start_x, 0, win.bar_ls_width, win.bar_ls_height, tga->width, tga->data, win_bar_ls);
    theme_crop_rect_into(win.start_x + win.bar_ls_width + 1, 0, win.bar_ms_width, win.bar_ms_height, tga->width, tga->data, win_bar_ms);
    theme_crop_rect_into(win.start_x + win.bar_ls_width + 2 + win.bar_ms_width, 0, win.bar_rs_width, win.bar_rs_height, tga->width, tga->data, win_bar_rs);
 
    theme_crop_rect_into(win.start_x, win.bar_ls_height + 1, win.ls_width, win.ls_height, tga->width, tga->data, win_ls);
    theme_crop_rect_into(win.start_x + win.ls_width + 2 + win.bar_ms_width, win.bar_ls_height + 1, win.rs_width, win.rs_height, tga->width, tga->data, win_rs);

    theme_crop_rect_into(win.start_x, win.bar_ls_height + 2 + win.ls_height, win.bottom_ls_width, win.bottom_ls_height, tga->width, tga->data, win_bottom_ls);
    theme_crop_rect_into(win.start_x + win.bottom_ls_width + 1, win.bar_ls_height + 2 + win.ls_height, win.bottom_ms_width, win.bottom_ms_height, tga->width, tga->data, win_bottom_ms);
    theme_crop_rect_into(win.start_x + win.bottom_ls_width + 2 + win.bottom_ms_width, win.bar_ls_height + 2 + win.ls_height, win.bottom_rs_width, win.bottom_rs_height, tga->width, tga->data, win_bottom_rs);
}