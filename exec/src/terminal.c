#include <string.h>
#include <colors.h>
#include <types.h>
#include <wm.h>
#include <flanterm.h>
#include <key.h>

window_info* win;
struct flanterm_context* ft_ctx;

u32 buf_idx = 0;
char buf[512];

void terminal_handle(char c) {
    wm_begin_draw(win);
    switch (c) {
        case '\n':
            flanterm_write(ft_ctx, "\n", 1);
            flanterm_write(ft_ctx, buf, buf_idx);
            for (int i = 0; i < buf_idx; i++) buf[i] = 0;
            flanterm_write(ft_ctx, "\nA:/> ", 6);
            buf_idx = 0;
            break;
        case '\b':
            if (buf_idx > 0) {
                flanterm_write(ft_ctx, "\b \b", 3);
                buf_idx--;
                buf[buf_idx] = 0;
            }
            break;
        default:
            buf[buf_idx++] = c;
            char str[1] = {c};
            flanterm_write(ft_ctx, str, 1);
            break;
    }
    wm_end_draw(win);
}

int main() {
    win = wm_create_window(50, 50, 80 * 8, 25 * 16, "Terminal");
    ft_ctx = flanterm_simple_init(win->fb->buffer, win->rect.width, win->rect.height, win->rect.width * 4);

    wm_begin_draw(win);
    for (int i = 0; i < win->rect.width * win->rect.height; i++) {
        win->fb->buffer[i] = 0;
    }

    flanterm_write(ft_ctx, "A:/> ", 5);

    wm_end_draw(win);
    char c = 0;
    bool has_data = false;
    while (true) {
        c = key_get(win);
        if (c != 0) {
            has_data = true;
            terminal_handle(c);
        } else {
            if (has_data) {
                key_clear(win);
                has_data = false;
            }
        }
    }
}