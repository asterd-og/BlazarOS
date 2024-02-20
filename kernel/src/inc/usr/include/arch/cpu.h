#pragma once

#include <types.h>

inline u64 rdmsr(u32 msr) {
    u32 low;
    u32 high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((u64)high << 32) | low;
}

inline void wrmsr(u32 msr, u64 val) {
    __asm__ volatile ("wrmsr" : : "a"((u32)val), "d"((u32)(val >> 32)), "c"(msr));
}

inline void get_cpuid(u32 reg, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx) {
    __asm__("cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "0" (reg));
}

void sse_init();