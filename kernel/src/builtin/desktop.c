#include <builtin/desktop.h>

#include <desktop/wm.h>
#include <desktop/window.h>

#include <desktop/elements/button.h>

#include <desktop/theme/theme_man.h>

#include <video/vbe.h>

#include <dev/timer/rtc/rtc.h>

#include <sched/sched.h>

void desktop_task() {
    u64 frame_counter = 0;
    u64 second_counter = 0;
    while (1) {
        wm_update();
        frame_counter++;
        if (second_counter != rtc_get(RTC_SECOND)) {
            second_counter = rtc_get(RTC_SECOND);
            serial_printf("FPS = %d\n", frame_counter);
            frame_counter = 0;
        }
    }
}

void desktop_init() {
    theme_init("theme.tga");
    btn_init();
    window_init();

    window_info* win2 = wm_create_window(190, 50, 450, 250, "Terminal2");
    window_info* win = wm_create_window(190, 150, 450, 250, "Terminal");
    element_info* btn = btn_create(25, 25, "Buttons!!", win);
    window_add_element(win, btn);

    wm_init();
    sched_new_proc(desktop_task, 1, PROC_PR_HIGH);
}