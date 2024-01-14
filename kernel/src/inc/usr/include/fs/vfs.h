#pragma once

#include <types.h>

#include <fs/fat32.h>

#include <flanterm/colors.h>

typedef struct {
    char* path;
    fat32_directory* current_dir;
} vfs_info;

void vfs_init();

u8* vfs_read(const char* path);
void vfs_write(const char* path, u8* buffer, u32 size);

void vfs_list_print();

int vfs_cd(const char* path);

extern vfs_info* vfs;