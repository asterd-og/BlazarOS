#pragma once

#include <types.h>
#include <desktop/rect.h>
#include <video/framebuffer.h>
#include <lib/atomic.h>

struct element_info;

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

void wm_bring_to_front(u8 win_idx);

char wm_get_key(window_info* win);
void wm_clear_key(window_info* win);

void wm_mouse_task();

enum {
    WM_CURSOR_NORMAL,
    WM_CURSOR_DRAGGING
};

extern framebuffer_info* wm_fb;
extern rectangle wm_mouse_rect;
extern wm_theme_info* wm_theme;
extern window_info* wm_window_list[128];
extern u8 wm_window_list_idx;
extern bool wm_redraw;
extern u8 wm_window_z_order[128];
extern bool wm_moving_window;
extern u8 wm_cursor_state;