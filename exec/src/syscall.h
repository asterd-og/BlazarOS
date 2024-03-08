#pragma once

static inline void syscall(u64 num, void* rdi, void* rsi, void* rdx, void* r10, void* r8) {
    asm volatile (
        "movq %0, %%rax;"
        "movq %1, %%rdi;"
        "movq %2, %%rsi;"
        "movq %3, %%rdx;"
        "movq %4, %%r10;"
        "movq %5, %%r8;"
        "int $0x80;"
        :
        : "r"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8)
        : "rax", "rdi", "rsi", "rdx", "r10", "r8"
    );
}
