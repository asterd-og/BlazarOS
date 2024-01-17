#pragma once

#include <types.h>

#include <mm/heap/heap.h>

enum {
    DEV_UNKNOWN,
    DEV_CHAR
};

struct dev_info {
    char name[24]; // E.g: fb0 = framebuffer 0 (dev:/fb0)
    u8 type;
    int (*read)(u8* buf, u32 offset, u32 size);
    int (*write)(u8* buf, u32 offset, u32 size);
};

typedef struct dev_info dev_info;

void dev_init(); // Initialises all basic devices, tty0, fb0, kb etc into vfs

void dev_register(const char* name, u8 type, int (*read)(u8*,u32,u32), int (*write)(u8*,u32,u32));

dev_info* dev_get(const char* name);

extern dev_info* dev_list[256];