#include <string.h>
#include <colors.h>
#include <types.h>
#include "wm.h"
#include "flanterm.h"

window_info* win;
struct flanterm_context* ft_ctx;

u32 buf_idx = 0;
char buf[512];

void terminal_handle(wm_message* msg) {
    switch (msg->type) {
        case WM_KEYBOARD_DOWN: {
            wm_begin_draw(win);
            char str[1] = {msg->content};
            flanterm_write(ft_ctx, str, 1);
            switch (msg->content) {
                case '\n':
                    flanterm_write(ft_ctx, buf, buf_idx);
                    for (int i = 0; i < buf_idx; i++) buf[i] = 0;
                    flanterm_write(ft_ctx, "\nA:/> ", 6);
                    buf_idx = 0;
                    break;
                case '\b':
                    if (buf_idx > 0) {
                        flanterm_write(ft_ctx, " \b", 2);
                        buf_idx--;
                        buf[buf_idx] = 0;
                    }
                    break;
                default:
                    buf[buf_idx++] = msg->content;
                    break;
            }
            wm_end_draw(win);
            break;
        }
    }
}

int main() {
    win = wm_create_window(50, 50, 650, 250, "Terminal");
    wm_event* event = win->event;

    ft_ctx = flanterm_simple_init(win->fb->buffer, win->rect.width, win->rect.height, win->rect.width * 4);

    wm_begin_draw(win);
    for (int i = 0; i < win->rect.width * win->rect.height; i++) {
        win->fb->buffer[i] = 0;
    }

    flanterm_write(ft_ctx, "A:/> ", 5);

    wm_end_draw(win);

    while (true) {
        switch (event->type) {
            case WM_EVENT_UPDATE:
                terminal_handle(event->message);
                wm_dispatch_event(event);
                break;
        }
    }
}