#pragma once

#include <types.h>

#include <fs/fat32.h>

#include <flanterm/colors.h>

#include <dev/dev.h>

#define VFS_PERM_NONE 0
#define VFS_PERM_READ 1
#define VFS_PERM_WRITE 2

enum {
    VFS_UNKNOWN,
    VFS_ROOT,
    VFS_DEV
};

typedef struct {
    char name[128];
    u8 type;
    u32 perm;
    u32 seek; // position
    u32 size;
} vfs_file;

struct vfs_info {
    char name[24]; // E.g = hd0: | dev:
    u8 type;
    int (*read)(struct vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset);
    int (*write)(struct vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset);
    vfs_file (*get)(struct vfs_info* vfs, const char* path);
};

typedef struct vfs_info vfs_info;

void vfs_init();

u8* vfs_read(vfs_info* vfs, const char* path);
void vfs_write(vfs_info* vfs, const char* path, u8* buffer, u32 size);

void vfs_list_print(vfs_info* vfs);

extern vfs_info* root_fs;
extern vfs_info* dev_fs;