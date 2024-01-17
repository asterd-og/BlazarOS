#pragma once

#include <types.h>

u64 syscall(int vec, void* b, void* c) {
    asm volatile("int $0x80\n\t"
        : "+a"(vec), "+b"(b), "+c"(c)
        :
        : "memory");
    
    u64 rax = 0;
    asm volatile("" : "=a"(rax) : : "memory");
    return rax;
}