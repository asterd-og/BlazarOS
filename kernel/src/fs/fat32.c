#include <fs/fat32.h>
#include <mm/heap/heap.h>

u32 fat_total_clusters = 0;
u32 fat_first_sector = 0;
u32 fat_data_sector = 0;
u32 fat_root_sector = 0;
u32 fat_file_count = 0;

fat32_bpb* fat_bpb;
fat32_ebpb* fat_ebpb;

fat32_directory* fat_root_dir;
fat32_entry* fat_entries;

fat32_directory* fat_directories_cache;

fat32_info* fat_info;

u32 fat32_get_sector(u32 cluster) {
    return ((cluster - 2) * fat_bpb->sectors_per_cluster) + fat_data_sector;
}

// TODO: Add directories cache and optimize write functions
// To not rewrite the entire directory table

char* fat32_process_name(fat32_entry* entry) {
    char* name = kmalloc(11);
    u8 i = 0;
    for (; i < 8; i++) {
        if (entry->name[i] == 0x20 || entry->name[i] == 0x10) {
            name[i] = 0;
            break;
        }
    }

    if (entry->attributes & FAT_ATTR_DIRECTORY) return name;

    name[i] = '.';
    i++;

    u8 j = 0;
    for (; j < 3; j++) {
        if (entry->name[8 + j] == 0x20 || entry->name[8 + j] == 0x10) {
            name[i + j] = 0;
            break;
        }
        name[i] = entry->name[8 + j];
        i++;
    }
    name[i] = 0;
    return name;
}

char* fat32_unprocess_name(const char* filename) {
    char* name = kmalloc(11);

    memset(name, 0x20, 11);

    u8 i = 0;
    for (; i < 8; i++) {
        if (filename[i] == '.' || filename[i] == 0) break;
        name[i] = toupper(filename[i]);
    }
    if (filename[i] == 0) {
        name[i] = 0;
        return name;
    }
    i++;

    for (u8 j = 10; j > 7; j--) {
        name[j] = toupper(filename[i]);
        i++;
    }

    return name;
}

fat32_directory* fat32_traverse_dir(fat32_directory* root_dir, const char* dirname) {
    for (u32 i = 0; i < root_dir->file_count; i++) {
        if (!memcmp(dirname, root_dir->entries[i].name, strlen(dirname))) {
            fat32_entry entry = root_dir->entries[i];
            if ((entry.attributes & FAT_ATTR_DIRECTORY) != FAT_ATTR_DIRECTORY) return NULL;
            u32 dir_cluster = ((u32)entry.high_cluster_entry << 16) | ((u32)entry.low_cluster_entry);
            u32 dir_sector = fat32_get_sector(dir_cluster);
            
            u8* mem = kmalloc(512);
            ata_read_multiple(dir_sector, 1, mem);
            
            fat32_entry* dir_entry;
            
            u32 loc = 0;
            u32 sector_count = 0;
            u32 file_count = 0;

            while (true) {
                if (loc * sizeof(fat32_entry) >= 512) {
                    loc = 0;
                    ata_read_multiple(dir_sector + sector_count, 1, mem);
                }
                dir_entry = (fat32_entry*)(mem + (loc * sizeof(fat32_entry)));
                if (dir_entry->name[0] == 0) break;
                loc++;
                if (dir_entry->attributes & FAT_ATTR_HIDDEN)
                    continue;
                file_count++;
            }

            fat32_directory* dir = kmalloc(sizeof(fat32_directory));

            dir->own_entry = &root_dir->entries[i];
            dir->parent = root_dir;
            u32 own_cluster = ((u32)entry.high_cluster_entry << 16)
                        | ((u32)entry.low_cluster_entry);
            dir->sector = fat32_get_sector(own_cluster);
            dir->cluster = own_cluster;
            loc = 0;
            sector_count = 0;
            memset(mem, 0, 512);
            dir->file_count = file_count;
            dir->entries = (fat32_entry*)kmalloc(sizeof(fat32_entry) * dir->file_count);
            ata_read_multiple(dir_sector, 1, mem);

            for (u32 j = 0; j < dir->file_count;) {
                if (loc * sizeof(fat32_entry) >= 512) {
                    loc = 0;
                    ata_read_multiple(dir_sector + sector_count, 1, mem);
                }
                dir_entry = (fat32_entry*)(mem + (loc * sizeof(fat32_entry)));
                loc++;
                if (dir_entry->attributes & FAT_ATTR_HIDDEN)
                    continue;
                memcpy(&dir->entries[j], dir_entry, sizeof(fat32_entry));
                j++;
            }

            kfree(mem);

            return dir;
        }
    }
    return NULL;
}

fat32_entry fat32_get_entry(fat32_directory* working_dir, const char* filename) { 
    for (u32 i = 0; i < working_dir->file_count; i++) {
        if (!memcmp(filename, working_dir->entries[i].name, strlen(filename))) {
            fat32_entry entry = working_dir->entries[i];
            if ((entry.attributes & FAT_ATTR_DIRECTORY) == FAT_ATTR_DIRECTORY) break;
            return entry;
        }
    }

    return (fat32_entry){0};
}

fat32_entry fat32_traverse_path(char* path) {
    int i = 0;
    u32 file_count = 0;
    for (i = 0; i < strlen(path); i++) {
        if (path[i] == '/') file_count++;
    }

    i = 0;

    char** arr = (char**)kmalloc(file_count * 11 + file_count);
    char* p = strtok(path, "/");

    while (p != NULL) {
        arr[i++] = p;
        p = strtok(NULL, "/");
    }

    fat32_directory* dir = fat_root_dir;

    for (int j = 0; j < i - 1; j++) {
        char* pt = fat32_unprocess_name(arr[j]);
        dir = fat32_traverse_dir(dir, pt);
        if (dir == NULL) {
            serial_printf("Oops, didn't find dir!\n");
            kfree(pt);
            kfree(arr);
            return (fat32_entry){0};
        }
        kfree(pt);
    }
    
    char* fname = fat32_unprocess_name(arr[i - 1]);
    fat32_entry entry = fat32_get_entry(dir, fname);
    kfree(fname);

    kfree(arr);

    return entry;
}

fat32_directory* fat32_find_entry_subdir(char* path) {
    int i = 0;
    u32 file_count = 0;
    for (i = 0; i < strlen(path); i++) {
        if (path[i] == '/') file_count++;
    }

    i = 0;

    char** arr = (char**)kmalloc(file_count * 8 + file_count);
    char* p = strtok(path, "/");

    while (p != NULL) {
        arr[i++] = p;
        p = strtok(NULL, "/");
    }

    fat32_directory* dir = fat_root_dir;

    for (u32 j = 0; j < file_count; j++) {
        char* pt = fat32_unprocess_name(arr[j]);
        dir = fat32_traverse_dir(dir, pt);
        kfree(pt);
    }

    kfree(arr);

    serial_printf("Inside dir: '%s'\n", dir->own_entry->name);

    return dir;
}

fat32_directory* fat32_find_subdir(const char* path_un) {
    char* path = kmalloc(strlen(path_un));
    memcpy(path, path_un, strlen(path_un));

    int i = 0;
    u32 file_count = 0;
    for (i = 0; i < strlen(path); i++) {
        if (path[i] == '/') file_count++;
    }
    i = 0;

    char** arr = (char**)kmalloc(file_count * 8 + file_count);
    char* p = strtok(path, "/");

    while (p != NULL) {
        arr[i++] = p;
        p = strtok(NULL, "/");
    }

    fat32_directory* dir = fat_root_dir;

    for (u32 j = 0; j < i; j++) {
        char* pt = fat32_unprocess_name(arr[j]);
        dir = fat32_traverse_dir(dir, pt);
        kfree(pt);
    }

    kfree(arr);

    serial_printf("Inside dir: '%s'\n", dir->own_entry->name);

    kfree(path);
    return dir;
}

int fat32_find_last_name(const char* path) {
    int i = strlen(path);
    while (path[i] != '/') {
        i--;
    }
    i++;
    return i;
}

fat32_entry* fat32_find_entry(fat32_directory* working_dir, const char* filename) {
    for (u32 i = 0; i < working_dir->file_count; i++) {
        if (!memcmp(filename, working_dir->entries[i].name, strlen(filename))) {
            return &working_dir->entries[i];
        }
    }
    return NULL;
}

int fat32_read(const char* filename, u8* buffer) {
    fat32_entry entry;
    bool in_dir = false;
    
    for (int i = 0; i < strlen(filename); i++) {
        if (filename[i] == '/') {
            char* name = kmalloc(strlen(filename));
            memcpy(name, filename, strlen(filename));
            entry = fat32_traverse_path(name);
            kfree(name);
            in_dir = true;
        }
    }

    if (!in_dir) {
        char* fname = fat32_unprocess_name(filename);
        entry = fat32_get_entry(fat_root_dir, fname);
        kfree(fname);
    }

    if (entry.name[0] == 0) return 1;
    
    u32 num_sectors = 1;
    u32 file_cluster = ((u32)entry.high_cluster_entry << 16)
                        | ((u32)entry.low_cluster_entry);
    u32 file_sector = fat32_get_sector(file_cluster);

    if (entry.size > 512) {
        num_sectors = ALIGN_UP(entry.size, 512);
        num_sectors /= 512;
    }

    ata_read_multiple(file_sector, num_sectors, buffer);

    return 0;
}

u32 fat32_allocate_cluster() {
    if (fat_info->available_cluster_num != 0xFFFFFFFF) {
        u32 ret = fat_info->available_cluster_num;
        u32 sector = fat32_get_sector(fat_info->available_cluster_num + 1);
        // Find next available cluster
        u8* cluster_buf = kmalloc(512);

        memset(cluster_buf, 0, 512);
        ata_read_multiple(sector, 1, cluster_buf);

        if (cluster_buf[0] != 0x00000000) {
            fat_info->available_cluster_num++;
            while (cluster_buf[0] != 0x00000000) {
                sector = fat32_get_sector(fat_info->available_cluster_num);
                ata_read_multiple(sector, 1, cluster_buf);
                fat_info->available_cluster_num++;
            }
            // Found free cluster!
        } else {
            fat_info->available_cluster_num++;
        }

        kfree(cluster_buf);
        ata_write_multiple(fat_ebpb->fsinfo_sector, 1, fat_info);
        // Flush fsinfo

        return ret;
    } else {
        if (fat_info->free_cluster_count == 0) return 0;
        for (u32 i = 2; i < fat_info->free_cluster_count; i++) {
            // TODO: Look for 0x00000000
        }
        return 0;
    }
}

void fat32_rewrite_directories(fat32_directory* working_dir) {
    u32 root_sect_count = 1;
    if (working_dir->file_count * sizeof(fat32_entry) > 512) {
        root_sect_count = ALIGN_UP(working_dir->file_count * sizeof(fat32_entry), 512);
        root_sect_count /= 512;
    }

    u8* copy_buf = kmalloc(root_sect_count * 512);
    memcpy(copy_buf, working_dir->entries, (working_dir->file_count * sizeof(fat32_entry)));

    ata_write_multiple(working_dir->sector, root_sect_count, copy_buf);

    kfree(copy_buf);
}

void fat32_cleanup_clusters(u32 cluster, u32 size) {
    u32 sect_count = 1;
    if (size > 512) {
        sect_count = ALIGN_UP(size, 512);
        sect_count /= 512;
    }

    u32 sector = fat32_get_sector(cluster);

    u8* buffer = kmalloc(sect_count * 512);
    memset(buffer, 0, sect_count * 512);

    ata_write_multiple(sector, sect_count, buffer);

    kfree(buffer);
}

/*
NOTE: FAT32_WRITE'S BUFFER ARGUMENT, HAS TO BE AN ALLOCATED
U8 WITH A MULTIPLE OF 512 BYTES IN SIZE! OR ELSE IT COULD
POINT TO ANOTHER FILE (Because of how sectors works)
*/

int fat32_write(const char* filename, u8* buffer, u32 size, u8 attributes) {
    fat32_directory* working_dir = fat_root_dir;
    char* fname = kmalloc(11);
    memset(fname, 0, 11);
    memcpy(fname, filename, 11);
    for (int i = 0; i < strlen(filename); i++) {
        if (filename[i] == '/') {
            char* name = kmalloc(strlen(filename));
            memcpy(name, filename, strlen(filename));
            working_dir = fat32_find_entry_subdir(name);
            int last_name = fat32_find_last_name(filename);
            memcpy(fname, filename + last_name, strlen(filename) - last_name);
            kfree(name);
        }
    }

    for (u32 i = 0; i < working_dir->file_count + 25; i++) {
        if ((u8)working_dir->entries[i].name[0] != 0xE5 &&
            (u8)working_dir->entries[i].name[0] != 0x0) {
                continue;
            }
        // Directory entry free!
        fat32_entry* entry = &working_dir->entries[i];

        char* entry_name = fat32_unprocess_name(fname);

        memcpy(entry->name, entry_name, 11);

        kfree(entry_name);
        kfree(fname);
        entry->attributes = attributes;

        u32 cluster = fat32_allocate_cluster();
        if (cluster == 0) {
            serial_printf("Couldn't find free cluster to allocate!\n");
            return 1;
        }

        u8 sec_count = 1;

        if (size > 512) {
            sec_count = ALIGN_UP(size, 512);
            sec_count /= 512;
        }

        entry->high_cluster_entry = (u16)(cluster >> 16);
        entry->low_cluster_entry = (u16)(cluster & 0xFFFF);

        entry->size = size;

        working_dir->file_count++;

        ata_write_multiple(fat32_get_sector(cluster), sec_count, buffer);
        fat32_rewrite_directories(working_dir);
        return 0;
    }
    kfree(fname);
    return 1;
}

int fat32_overwrite(const char* filename, u8* buffer, u32 size, u8 attributes) {
    fat32_directory* working_dir = fat_root_dir;
    char* fname = kmalloc(11);
    memset(fname, 0, 11);
    memcpy(fname, filename, 11);
    for (int i = 0; i < strlen(filename); i++) {
        if (filename[i] == '/') {
            char* name = kmalloc(strlen(filename));
            memcpy(name, filename, strlen(filename));
            working_dir = fat32_find_entry_subdir(name);
            int last_name = fat32_find_last_name(filename);
            memcpy(fname, filename + last_name, strlen(filename) - last_name);
            kfree(name);
        }
    }

    char* entry_name = fat32_unprocess_name(fname);
    fat32_entry* entry = fat32_find_entry(working_dir, entry_name);
    kfree(entry_name);

    if (entry->name[0] == 0) return 1;

    u32 sect_count = 1;

    if (size > 512) {
        sect_count = ALIGN_UP(size, 512);
        sect_count /= 512;
    }

    u32 sector = 0;

    if (entry->size <= size) {
        // We can just reuse entry's cluster.
        u32 cluster = ((u32)entry->high_cluster_entry << 16)
                    | ((u32)entry->low_cluster_entry);
        
        sector = fat32_get_sector(cluster);
    } else {
        // Allocate new fat32 cluster to use that.
        u32 old_cluster = ((u32)entry->high_cluster_entry << 16)
                    | ((u32)entry->low_cluster_entry);
        fat32_cleanup_clusters(old_cluster, entry->size);

        u32 cluster = fat32_allocate_cluster();
        sector = fat32_get_sector(cluster);

        entry->high_cluster_entry = (u16)(cluster >> 16);
        entry->low_cluster_entry = (u16)(cluster & 0xFFFF);     
    }

    entry->size = size;
    ata_write_multiple(sector, sect_count, buffer);
    fat32_rewrite_directories(working_dir);

    kfree(fname);
    return 0;
}

int fat32_create_dot(fat32_directory* working_dir) {
    bool dot_written = false; // set to true once we create '.'
    for (u32 i = 0; i < 2; i++) {
        fat32_entry* entry = &working_dir->entries[i];

        char* name = (dot_written ? "..      " : ".       ");
        serial_printf("Creating '%s' on %s.\n", name, working_dir->own_entry->name);

        memcpy(entry->name, name, 11);
        entry->attributes = FAT_ATTR_DIRECTORY;

        u32 cluster = (dot_written ? working_dir->parent->cluster : working_dir->cluster);

        entry->high_cluster_entry = (u16)(cluster >> 16);
        entry->low_cluster_entry = (u16)(cluster & 0xFFFF);

        working_dir->file_count++;

        entry->size = 0;

        if (dot_written) {
            fat32_rewrite_directories(working_dir);
            return 0;
        }

        dot_written = true;
    }
    return 1;
}

int fat32_create_dir(const char* path_processed) {
    fat32_directory* working_dir = fat_root_dir;
    char* path = kmalloc(11);
    memset(path, 0, 11);
    memcpy(path, path_processed, 11);
    for (int i = 0; i < strlen(path_processed); i++) {
        if (path_processed[i] == '/') {
            char* name = kmalloc(strlen(path_processed));
            memcpy(name, path_processed, strlen(path_processed));
            working_dir = fat32_find_entry_subdir(name);
            int last_name = fat32_find_last_name(path_processed);
            memcpy(path, path_processed + last_name, strlen(path_processed) - last_name);
            kfree(name);
        }
    }

    // Find free entry

    for (u32 i = 0; i < working_dir->file_count; i++) {
        if ((u8)working_dir->entries[i].name[0] != 0xE5 &&
            (u8)working_dir->entries[i].name[0] != 0x0) {
                continue;
            }
        
        fat32_entry* entry = &working_dir->entries[i];

        char* entry_name = fat32_unprocess_name(path);
        memcpy(entry->name, entry_name, 11);
        serial_printf("%s vs %s vs %s", entry_name, entry->name, path);

        entry->attributes = FAT_ATTR_DIRECTORY;

        u32 cluster = fat32_allocate_cluster();
        if (cluster == 0) {
            serial_printf("Couldn't allocate cluster for directory!\n");
        }

        entry->high_cluster_entry = (u16)(cluster >> 16);
        entry->low_cluster_entry = (u16)(cluster & 0xFFFF);

        entry->size = 0;

        u8* buf0 = kmalloc(fat_bpb->sectors_per_cluster * 512);
        memset(buf0, 0, fat_bpb->sectors_per_cluster * 512);
        *((u32*)buf0 + ((fat_bpb->sectors_per_cluster * 512) - 2)) = FAT_END_CLUSTER;
        ata_write_multiple(fat32_get_sector(cluster), fat_bpb->sectors_per_cluster, buf0);
        kfree(buf0);

        working_dir->file_count++;

        if (fat32_create_dot(fat32_traverse_dir(working_dir, entry_name))) {
            serial_printf("Couldn't create dot directories!\n");
        }
        kfree(entry_name);

        fat32_rewrite_directories(working_dir);
        return 0;
    }

    kfree(path);
}

void fat32_init() {
    u8* mem = kmalloc(512);
    ata_read_multiple(0, 1, mem);
    fat32_bpb* bpb = (fat32_bpb*)mem;
    fat32_ebpb* ebpb = (fat32_ebpb*)bpb->extended_section;

    if (mem[510] != 0x55 || mem[511] != 0xAA)
        log_bad("FAT: Bad fat32\n");
    else
        log_ok("FAT: Detect fat32!\n");
    
    u8* fsinfo_mem = kmalloc(512);
    ata_read_multiple(ebpb->fsinfo_sector, 1, fsinfo_mem);
    fat_info = (fat32_info*)fsinfo_mem;

    if (fat_info->lead_signature != 0x41615252) {
        log_bad("FAT: Bad FSInfo section!\n");
    }

    log_info("FAT: Free cluster count: %d\n", fat_info->free_cluster_count);
    log_info("FAT: Next free cluster: %d\n", fat_info->available_cluster_num);
    
    fat_total_clusters = bpb->total_sectors_32 / bpb->sectors_per_cluster;

    fat_data_sector = bpb->resv_sectors + (bpb->fat_count * ebpb->table_size_32);
    fat_first_sector = bpb->resv_sectors;
    fat_root_sector = fat32_get_sector(ebpb->root_cluster);

    fat_bpb = bpb;
    fat_ebpb = ebpb;

    u8* mem2 = kmalloc(512);
    ata_read_multiple(fat_root_sector, 1, mem2);
    fat32_entry* dir;
    fat_root_dir = (fat32_directory*)kmalloc(sizeof(fat32_directory));
    u32 loc = 0;
    u32 sector_count = 0;
    fat_file_count = 0;
    
    fat_root_dir->sector = fat_root_sector;
    fat_root_dir->cluster = ebpb->root_cluster;

    while (true) {
        if (loc * sizeof(fat32_entry) >= 512) {
            loc = 0;
            ata_read_multiple(fat_root_sector + sector_count, 1, mem2);
        }
        dir = (fat32_entry*)(mem2 + (loc * sizeof(fat32_entry)));
        loc++;
        if (dir->name[0] == 0) break;
        if (dir->attributes & FAT_ATTR_HIDDEN)
            continue;
        fat_file_count++;
    }

    fat_entries = (fat32_entry*)kmalloc(sizeof(fat32_entry) * fat_file_count);
    fat_root_dir->file_count = fat_file_count;
    fat_root_dir->entries = (fat32_entry*)kmalloc(sizeof(fat32_entry) * fat_file_count);

    memset(mem2, 0, 512);

    loc = 0;
    sector_count = 0;

    ata_read_multiple(fat_root_sector, 1, mem2);
    
    for (u32 j = 0; j < fat_root_dir->file_count;) {
        if (loc * sizeof(fat32_entry) >= 512) {
            loc = 0;
            ata_read_multiple(fat_root_sector + sector_count, 1, mem2);
        }
        dir = (fat32_entry*)(mem2 + (loc * sizeof(fat32_entry)));
        loc++;
        if (dir->attributes & FAT_ATTR_HIDDEN) continue;
    
        memcpy(&fat_root_dir->entries[j], dir, sizeof(fat32_entry));
        j++;
    }
}
