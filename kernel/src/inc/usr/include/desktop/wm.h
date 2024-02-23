#pragma once

#include <types.h>
#include <desktop/rect.h>
#include <video/framebuffer.h>

/*
WM IDEA:

window_info* win = wm_create_window(x, y, w, h, name);
event_info* event = wm_create_event(win);

element_info* button = wm_create_element(WM_BUTTON, x, y, w, h, contents);
// elemnts can also have events

window_add_element(win, button);

while (1) {
    wm_get_event(event);
    wm_dispatch_event(event);
    switch (event->type) {
        case WM_QUIT:
            // Free all the objects that we needa free
            return 0;
        case WM_UPDATE:
            update(win, event); // Mouse/Keyboard event(possibly?)
            break;
    }
}

*/

struct element_info;

typedef struct {
    rectangle rect;

    u32 pitch;
    u32* buffer;
    framebuffer_info* fb;
    
    char* title;
    int title_len;

    bool dirty;

    struct element_info** elements;
    int element_count;
} window_info;

typedef struct {
    rectangle rect;
    bool dirty;

    u8* data;

    window_info* parent;
} element_info;

typedef struct {
    u32 window_title;
    u32 window_title_font;

    u32 mouse_outline;
    u32 mouse_inline;

    u32 background;
} wm_theme_info;

void wm_init();

window_info* wm_create_window(int x, int y, int width, int height, char* title);

void wm_update();
void wm_draw();

framebuffer_info* wm_fb;
extern rectangle wm_mouse_rect;
extern wm_theme_info* wm_theme;