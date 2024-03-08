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

    wm_init();
    sched_new_proc(desktop_task, 1, PROC_PR_HIGH);
    u8* elf_buf = kmalloc(blazfs_ftell("terminal"));
    blazfs_read("terminal", elf_buf);
    sched_new_elf(elf_buf, 1, PROC_PR_LOW);
    sched_new_elf(elf_buf, 1, PROC_PR_LOW);
}