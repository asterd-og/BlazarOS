#include <fs/vfs.h>

vfs_info* hd_fs;
vfs_info* dev_fs;
vfs_info* root_fs;
vfs_info* vfs_list[256];
int vfs_count = 0;
bool vfs_test;

void vfs_lock() {
    if (vfs_test) return;
    while (__atomic_test_and_set(&vfs_test, __ATOMIC_ACQUIRE));
}

void vfs_unlock() {
    if (!vfs_test) return;
    __atomic_clear(&vfs_test, __ATOMIC_RELEASE);
}

int vfs_hd_read(vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset) {
    return 0;
}

int vfs_hd_write(vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset) {
    return 0;
}

/*

DEVICES (fb0, tty0 etc)

*/

int vfs_dev_read(vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset) {
    dev_info* device = dev_get(path);

    if (device == NULL)
        return 1;
    
    int status = device->read(buf, offset, size);

    return status;
}

int vfs_dev_write(vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset) {
    dev_info* device = dev_get(path);

    if (device == NULL)
        return 1;

    int status = device->write(buf, offset, size);

    return status;
}

/*

Wrappers for the root_fs (so I can call root_fs->read("hd0:/hey.txt") or root_fs->read("dev:/tty0"))

*/

int vfs_rfs_read(vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset) {
    int i = 0;
    for (; i < 24; i++) {
        if (path[i] == ':') break;
    }

    for (int j = 0; j < vfs_count; j++) {
        if (!memcmp(vfs_list[j]->name, path, i)) {
            return vfs_list[j]->read(vfs_list[j], path + i + 2, buf, size, offset);
        }
    }

    return -1;
}

int vfs_rfs_write(vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset) {
    int i = 0;
    for (; i < 24; i++) {
        if (path[i] == ':') break;
    }

    for (int j = 0; j < vfs_count; j++) {
        if (!memcmp(vfs_list[j]->name, path, i)) {
            return vfs_list[j]->write(vfs_list[j], path + i + 2, buf, size, offset);
        }
    }

    return -1;
}

/*

General Wrappers

*/

int vfs_read(const char* path, u8* buf, u32 size, u32 offset) {
    return root_fs->read(root_fs, path, buf, size, offset);
}

int vfs_write(const char* path, u8* buf, u32 size, u32 offset) {
    return root_fs->write(root_fs, path, buf, size, offset);
}

void vfs_init() {
    // Create HD fs
    hd_fs = (vfs_info*)kmalloc(sizeof(vfs_info));
    hd_fs->type = VFS_BLOCK;
    const char hd_name[] = "hd0";
    memcpy(hd_fs->name, hd_name, strlen(hd_name));

    hd_fs->read = vfs_hd_read;
    hd_fs->write = vfs_hd_write;
    vfs_list[vfs_count] = hd_fs;
    vfs_count++;

    // Dev fs
    dev_fs = (vfs_info*)kmalloc(sizeof(vfs_info));
    dev_fs->type = VFS_DEV;
    const char dev_name[] = "dev";
    memcpy(dev_fs->name, dev_name, strlen(dev_name));

    dev_fs->read = vfs_dev_read;
    dev_fs->write = vfs_dev_write;
    vfs_list[vfs_count] = dev_fs;
    vfs_count++;

    // Root fs
    root_fs = (vfs_info*)kmalloc(sizeof(vfs_info));
    root_fs->type = VFS_ROOT;

    root_fs->read = vfs_rfs_read;
    root_fs->write = vfs_rfs_write;
}