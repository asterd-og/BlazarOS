#pragma once

#include <types.h>
#include "syscall.h"

char key_get() {
    syscall(5, 0, 0, 0, 0, 0);
    char rax;
    asm volatile("" : "=a"(rax) : : "memory");
    return rax;
}

void key_clear() {
    syscall(6, 0, 0, 0, 0, 0);
}