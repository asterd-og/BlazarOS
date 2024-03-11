#include <sys/syscall.h>
#include <dev/ps2/keyboard.h>
#include <desktop/wm.h>
#include <desktop/window.h>

#include <desktop/elements/button.h>

#include <desktop/theme/theme_man.h>

#include <video/vbe.h>

#include <dev/timer/rtc/rtc.h>

#include <sched/sched.h>
#include <dev/serial/serial.h>

#include <flanterm/flanterm.h>

#include <lib/atomic.h>

locker_info syscall_lock;

void syscall_handler(registers* regs) {
    lock(&syscall_lock);
    switch (regs->rax) {
        case 0:
            // wm create new window
            regs->rax = wm_create_window(regs->rdi, regs->rsi, regs->rdx, regs->r10, (char*)regs->r8);
            break;
        case 1:
            // wm begin draw
            wm_begin_draw((window_info*)regs->rdi);
            break;
        case 2:
            // wm end draw
            wm_end_draw((window_info*)regs->rdi);
            break;
        case 3:
            // flanterm simple init
            regs->rax = flanterm_fb_simple_init((u32*)regs->rdi, regs->rsi, regs->rdx, regs->r10);
            break;
        case 4:
            // flanterm write
            flanterm_write((struct flanterm_context*)regs->rdi, (const char*)regs->rsi, regs->rdx);
            break;
        case 5:
            // key_get
            regs->rax = wm_get_key((window_info*)regs->rdi);
            break;
        case 6:
            // key_clear
            wm_clear_key((window_info*)regs->rdi);
            break;
        case 7:
            // font_get
            regs->rax = font_get(regs->rdi);
            break;
    }
    unlock(&syscall_lock);
}