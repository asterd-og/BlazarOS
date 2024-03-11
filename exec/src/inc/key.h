#pragma once

#include <types.h>
#include "syscall.h"
#include <desktop/wm.h>

char key_get(window_info* win) {
    syscall(5, (void*)win, 0, 0, 0, 0);
    char rax;
    asm volatile("" : "=a"(rax) : : "memory");
    return rax;
}

void key_clear(window_info* win) {
    syscall(6, (void*)win, 0, 0, 0, 0);
}