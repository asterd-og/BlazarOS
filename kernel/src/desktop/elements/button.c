#include <desktop/elements/button.h>
#include <mm/heap/heap.h>
#include <dev/ps2/mouse.h>
#include <desktop/theme/theme_man.h>

u32* btn_ls;
u32* btn_ms;
u32* btn_rs;

void btn_draw(element_info* btn) {
    theme_button_info* inf = &theme_global->button_info;
    button_info* btn_info = (button_info*)btn->data;

    fb_draw_fake_alpha_buffer(vbe, btn->rect.x, btn->rect.y, inf->ls_width, inf->ls_height, 0xFF00FF00, btn_ls);
    
    for (int i = 0; i < btn_info->text_len; i++)
        fb_draw_buffer(vbe, (btn->rect.x + (inf->ls_width + (inf->ms_width * i))), btn->rect.y, inf->ms_width, inf->ms_height, btn_ms);
    
    fb_draw_fake_alpha_buffer(vbe, (btn->rect.x + (inf->ls_width + (inf->ms_width * (btn_info->text_len - 1)))) + inf->ms_width, btn->rect.y, inf->rs_width, inf->rs_height, 0xFF00FF00, btn_rs);

    fb_draw_str(vbe, btn->rect.x + inf->ls_width, btn->rect.y + 2, 0xFF000000, btn_info->text, kernel_font);
}

void btn_update(element_info* btn) {
    button_info* btn_info = (button_info*)btn->data;
    btn_info->pressed = (rect_colliding_coord(wm_mouse_rect.x, wm_mouse_rect.y, btn->rect.x + btn->parent->rect.x, btn->rect.y + btn->parent->rect.y, btn->rect.width, btn->rect.height) && mouse_left_pressed);
}

element_info* btn_create(u32 x, u32 y, u32 w, u32 h, char* text, window_info* win) {
    element_info* btn = (element_info*)kmalloc(sizeof(element_info));
    btn->rect.x = x;
    btn->rect.y = y;
    btn->rect.width = w;
    btn->rect.height = h;

    btn->dirty = true;
    btn->parent = win;

    button_info* info = (button_info*)kmalloc(sizeof(button_info));
    info->pressed = false;
    info->text = (char*)kmalloc(strlen(text));
    info->text_len = strlen(text);
    memcpy(info->text, text, strlen(text));

    btn->data = (u8*)info;

    return btn;
}

void btn_init() {
    tga_info* tga = theme_global->image;
    theme_button_info btn = theme_global->button_info;

    btn_ls = (u32*)kmalloc(btn.ls_width * btn.ls_height * 4);
    btn_ms = (u32*)kmalloc(btn.ms_width * btn.ms_width * 4);
    btn_rs = (u32*)kmalloc(btn.rs_width * btn.rs_width * 4);

    theme_crop_rect_into(0, 0, btn.ls_width, btn.ls_height, tga->width, tga->data, btn_ls);
    theme_crop_rect_into(btn.ls_width + 2, 0, btn.ms_width, btn.ms_height, tga->width, tga->data, btn_ms);
    theme_crop_rect_into((btn.ls_width + 2) + btn.ms_width + 2, 0, btn.rs_width, btn.rs_height, tga->width, tga->data, btn_rs);
}