#pragma once

#include <types.h>
#include <desktop/rect.h>
#include <video/framebuffer.h>

/*
WM IDEA:

void window_handle(wm_message* msg) {
    switch (msg->type) {
        case WM_KEYBOARD_DOWN:
            printf("%c", msg->content);
            break;
        case WM_MOUSE_DOWN:
            printf("Mouse down! %s button\n", (msg->content == 1 ? "left" : "right"));
            break;
    }
}

window_info* win = wm_create_window(x, y, w, h, name);
event_info* event = win->event;

element_info* button = wm_create_element(WM_BUTTON, x, y, w, h, contents);
// elemnts can also have events

window_add_element(win, button);

while (1) {
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

enum {
    WM_MESSAGE_NONE,
    WM_KEYBOARD_DOWN
};

enum {
    WM_EVENT_NONE,
    WM_EVENT_QUIT,
    WM_EVENT_UPDATE
};

typedef struct {
    u8 type;
    u8 content;
} wm_message;

typedef struct {
    u8 type;
    wm_message* message;
} wm_event;

typedef struct {
    rectangle rect;
    u32 offx;
    u32 offy;
    bool moving;

    u8 win_idx;

    u32 pitch;
    u32* buffer;
    framebuffer_info* fb;
    bool fb_dirty;
    
    char* title;
    int title_len;

    bool dirty;

    struct element_info** elements;
    int element_count;

    wm_event* event;
} window_info;

struct element_info {
    rectangle rect;
    bool dirty;

    u8* data;

    window_info* parent;

    void(*draw)(struct element_info*);
    void(*update)(struct element_info*);
};

typedef struct element_info element_info;

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

void wm_begin_draw(window_info* win);
void wm_end_draw(window_info* win);

void wm_dispatch_event(wm_event* event);

void wm_bring_to_front(u8 win_idx);

extern framebuffer_info* wm_fb;
extern rectangle wm_mouse_rect;
extern wm_theme_info* wm_theme;
extern window_info* wm_window_list[128];
extern u8 wm_window_list_idx;
extern bool wm_redraw;
extern u8 wm_window_z_order[128];
extern bool wm_moving_window;