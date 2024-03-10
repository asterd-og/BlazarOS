#include <desktop/wm.h>
#include <desktop/window.h>
#include <types.h>
#include "syscall.h"

window_info* wm_create_window(int x, int y, int width, int height, char* title) {
    syscall(0, x, y, width, height, (void*)title);
    window_info* rax = 0;
    asm volatile("" : "=a"(rax) : : "memory");
    return rax;
}

void wm_begin_draw(window_info* win) {
    syscall(1, win, 0, 0, 0, 0);
}

void wm_end_draw(window_info* win) {
    syscall(2, win, 0, 0, 0, 0);
}