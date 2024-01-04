#pragma once

#include <main.h>

#define PAGE_SIZE 4096

#define DIV_ROUND_UP(x, y) (x + (y - 1)) / y
#define ALIGN_UP(x, y) DIV_ROUND_UP(x, y) * y
#define ALIGN_DOWN(x, y) (x / y) * y

#define HIGHER_HALF(ptr) ((void*)((u64)ptr) + hhdm_offset)
#define PHYSICAL(ptr) ((void*)((u64)ptr) - hhdm_offset)

#define PTE_PRESENT 1ull
#define PTE_WRITABLE (1ull << 1)
#define PTE_USER (1ull << 2)
#define PTE_NX (1ull << 63)

#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)
#define PTE_GET_FLAGS(VALUE) ((VALUE) & ~PTE_ADDR_MASK)

inline void write_cr3(u64 val) {
    __asm__ ("mov %0, %%cr3" : : "r"(val) : "memory");
}

inline u64 read_cr3() {
    u64 val;
    __asm__ ("mov %%cr3, %0" : "=r"(val) : : "memory");
    return val;
}

static inline void mmio_write32(void* addr, u32 val) {
    *((volatile u32*)addr) = val;
}

static inline u32 mmio_read32(void* addr) {
    return *((volatile u32*)addr);
}