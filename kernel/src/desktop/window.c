#include <desktop/window.h>
#include <mm/heap/heap.h>

void window_draw_decorations(window_info* win) {
    fb_blit_fb(vbe, win->fb, win->rect.x, win->rect.y);
}

void window_add_element(window_info* win, element_info* element) {
    win->elements[win->element_count++] = element;
}

window_info* window_create(u32 x, u32 y, u32 width, u32 height, char* name) {
    window_info* win = (window_info*)kmalloc(sizeof(window_info));
    win->title = (char*)kmalloc(strlen(name));
    memcpy(win->title, name, strlen(name));

    win->rect.x = x;
    win->rect.y = y;
    win->rect.width = width;
    win->rect.height = height;

    win->buffer = (u32*)kmalloc(width * height * 4);
    win->pitch = vbe->pitch;

    win->elements = (element_info**)kmalloc(sizeof(element_info) * 256);
    memset(win->elements, 0, sizeof(element_info) * 256);
    win->element_count = 0;

    win->fb = fb_create(win->buffer, width, height, win->pitch);
    fb_clear(win->fb, 0xff808080);

    return win;
}