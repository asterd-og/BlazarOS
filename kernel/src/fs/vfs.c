#include <fs/vfs.h>

vfs_info* vfs;

void vfs_init() {
    vfs = (vfs_info*)kmalloc(sizeof(vfs_info));
    vfs->path = kmalloc(512);
    char path[] = "/";
    memcpy(vfs->path, path, strlen(path));
    vfs->current_dir = fat_root_dir;
}

char* vfs_clean_path(const char* path) {
    char* mod_path;
    if (path[0] != '/') {
        mod_path = kmalloc(strlen(vfs->path) + strlen(path) + 2);
        if (strlen(vfs->path) > 2) {
            serial_printf("PEAO");
            memcpy(mod_path, vfs->path + 1, strlen(vfs->path) - 1);
            mod_path[strlen(vfs->path) - 1] = '/';
            memcpy(mod_path + strlen(vfs->path), path, strlen(path));
        } else {
            memcpy(mod_path, path, strlen(path));
        }
    } else {
        mod_path = kmalloc(strlen(path) + 2);
        memcpy(mod_path, path + 1, strlen(path) - 1);
    }
    return mod_path;
}

u8* vfs_read(const char* path) {
    char* mod_path = vfs_clean_path(path);

    fat32_entry* entry = fat32_get_absolute_entry(mod_path);

    if (entry == NULL) {
        serial_printf("VFS: Couldn't find file '%s'\n", mod_path);
        return NULL;
    }

    serial_printf("VFS: Opening file '%s'\n", mod_path);

    u8* buffer = kmalloc(ALIGN_UP(entry->size, 512));
    int status = fat32_read(mod_path, buffer);

    if (status != 0) {
        serial_printf("VFS: Couldn't open file '%s' err %d\n", mod_path, status);
        kfree(mod_path);
        kfree(buffer);
        return NULL;
    }

    kfree(mod_path);
    return buffer;
}

void vfs_write(const char* path, u8* buffer, u32 size) {
    char* mod_path = vfs_clean_path(path);

    serial_printf("VFS: %s\n", mod_path);

    fat32_entry* entry = fat32_get_absolute_entry(mod_path);

    u8* good_buf = kmalloc(ALIGN_UP(size, 512));
    memcpy(good_buf, buffer, size);

    if (entry == NULL) {
        // File doesn't exists, so create a new one
        fat32_write(mod_path, good_buf, size, 0);
    } else {
        fat32_overwrite(mod_path, good_buf, size, 0);
    }

    kfree(good_buf);
    kfree(mod_path);
}

void vfs_list_print() {
    printf("Listing '%s':", vfs->path);
    char* name_buf;
    fat32_entry* entry;
    for (int i = 0; i < vfs->current_dir->file_count; i++) {
        entry = &vfs->current_dir->entries[i];
        name_buf = fat32_process_name(entry);
        if (entry->attributes & FAT_ATTR_DIRECTORY)
            printf(COL_CYAN "\n\tDIR - %s" COL_WHITE, name_buf);
        else
            printf(COL_WHITE "\n\tFILE - %s | %dB", name_buf, entry->size);
        kfree(name_buf);
    }
    printf("\n");
}

int vfs_cd(const char* path) {
    if (path[0] == '.') {
        if (path[1] == '.') {
            if (vfs->current_dir != fat_root_dir) {
                int j = 0;
                int i = 0;
                for (i = strlen(vfs->path); i > 0; i--) {
                    if (vfs->path[i] == '/')
                        break;
                    j++;
                }
                if (vfs->current_dir->parent != fat_root_dir)
                    memset(vfs->path + i, 0, j);
                else
                    memset(vfs->path + (i + 1), 0, j);
                vfs->current_dir = vfs->current_dir->parent;
            }
            return 0;
        }
        return 0;
    }
    char* mod_path = vfs_clean_path(path);

    serial_printf("VFS: Mod path: '%s'\n", mod_path);

    fat32_directory* dir = fat32_find_subdir(mod_path);

    if (dir == NULL) {
        serial_printf("VFS: Couldn't find path '%s'.\n", path);
        return 1;
    }

    if (path[0] != '/') {
        if (vfs->current_dir != fat_root_dir) {
            vfs->path[strlen(vfs->path)] = '/';
        }
        memcpy(vfs->path + strlen(vfs->path), path, strlen(path));
    }
    vfs->current_dir = dir;

    serial_printf("VFS: New path: '%s'\n", vfs->path);
    return 0;
}