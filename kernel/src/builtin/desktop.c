#include <builtin/desktop.h>

#include <desktop/wm.h>
#include <desktop/window.h>

#include <desktop/elements/button.h>

#include <desktop/theme/theme_man.h>

#include <video/vbe.h>

#include <dev/timer/rtc/rtc.h>

#include <sched/sched.h>
#include <dev/serial/serial.h>

#include <flanterm/flanterm.h>

#include <dev/initrd/blazfs.h>

#include <lib/stdio/printf.h>

void desktop_task() {
    while (1) {
        wm_update();
    }
}

void win_fps() {
    u64 frame_counter = 0;
    u64 second_counter = 0;
    window_info* win = wm_create_window(50, 50, 8 * 9, 16, "FPS");
    fb_clear(win->fb, 0);
    wm_end_draw(win);
    char str[100];

    while (1) {
        frame_counter++;
        if (second_counter != rtc_get(RTC_SECOND)) {
            second_counter = rtc_get(RTC_SECOND);
            fb_clear(win->fb, 0);
            sprintf(str, "%d", frame_counter);
            fb_draw_str(win->fb, 0, 0, 0xFFFF00FF, str, kernel_font);
            wm_end_draw(win);
            frame_counter = 0;
        }
    }
}

void desktop_init() {
    theme_init("theme.tga");
    btn_init();
    window_init();

    wm_init();
    sched_new_proc(desktop_task, 0, PROC_PR_HIGH);
    sched_new_proc(win_fps, 1, PROC_PR_HIGH);
    // u8* elf_buf = kmalloc(blazfs_ftell("terminal"));
    // blazfs_read("terminal", elf_buf);
    // sched_new_elf(elf_buf, 1, PROC_PR_HIGH);
}