#pragma once

#include <flanterm/flanterm.h>
#include "syscall.h"

struct flanterm_context *flanterm_simple_init(
    uint32_t *framebuffer, size_t width, size_t height, size_t pitch) {
        syscall(4, (void*)framebuffer, width, height, pitch, 0);
        struct flanterm_context* rax = 0;
        asm volatile("" : "=a"(rax) : : "memory");
        return rax;
    }

void flanterm_write(struct flanterm_context* ctx, const char* str, size_t len) {
    syscall(5, (void*)ctx, (void*)str, len, 0, 0);
}