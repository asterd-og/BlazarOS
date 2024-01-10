#pragma once

#include <types.h>

static inline void outb(u16 port, u8 val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline u8 inb(u16 port) {
    u8 ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline void outw(u16 port, u16 val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline u16 inw(u16 port) {
    u16 ret;
    __asm__ volatile ("inw %%dx, %%ax" : "=a"(ret) : "d"(port) : "memory");
    return ret;
}