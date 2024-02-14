#include <desktop/elements/button.h>
#include <mm/heap/heap.h>
#include <dev/ps2/mouse.h>

void btn_draw(element_info* btn) {
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
    memcpy(info->text, text, strlen(text));

    btn->data = (u8*)info;

    return btn;
}