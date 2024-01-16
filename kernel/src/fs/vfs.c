#include <fs/vfs.h>

vfs_info* root_fs;
vfs_info* dev_fs;
bool vfs_test;

void vfs_lock() {
    if (vfs_test) return;
    while (__atomic_test_and_set(&vfs_test, __ATOMIC_ACQUIRE));
}

void vfs_unlock() {
    if (!vfs_test) return;
    __atomic_clear(&vfs_test, __ATOMIC_RELEASE);
}

int vfs_rfs_read(vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset) {
    u8* good_buf = kmalloc(ALIGN_UP(size, 512) / 512);
    
    int status = fat32_read(path, good_buf);

    if (status != 0) {
        kfree(good_buf);
        return 1;
    }

    memcpy(buf, good_buf, size);

    kfree(good_buf);
    return 0;
}

int vfs_rfs_write(vfs_info* vfs, const char* path, u8* buf, u32 size, u32 offset) {
    u8* good_buf = kmalloc(ALIGN_UP(size, 512) / 512);
    memcpy(good_buf, buf, size);
    
    int status = fat32_write(path, good_buf, size, FAT_ATTR_ARCHIVE);

    if (status != 0) {
        kfree(good_buf);
        return 1;
    }

    kfree(good_buf);
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

void vfs_init() {
    // Create root fs
    root_fs = (vfs_info*)kmalloc(sizeof(vfs_info));
    root_fs->type = VFS_ROOT;
    const char rfs_name[] = "hd0";
    memcpy(root_fs->name, rfs_name, strlen(rfs_name));

    root_fs->read = vfs_rfs_read;
    root_fs->write = vfs_rfs_write;

    // Dev fs
    dev_fs = (vfs_info*)kmalloc(sizeof(vfs_info));
    dev_fs->type = VFS_DEV;
    const char dev_name[] = "dev";
    memcpy(dev_fs->name, dev_name, strlen(dev_name));

    dev_fs->read = vfs_dev_read;
    dev_fs->write = vfs_dev_write;
}