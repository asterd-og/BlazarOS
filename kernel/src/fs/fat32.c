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

            dir->own_entry = root_dir->entries[i];
            u32 own_cluster = ((u32)entry.high_cluster_entry << 16)
                        | ((u32)entry.low_cluster_entry);
            dir->sector = fat32_get_sector(own_cluster);
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

fat32_directory* fat32_find_subdir(char* path) {
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

    for (int j = 0; j < file_count; j++) {
        char* pt = fat32_unprocess_name(arr[j]);
        dir = fat32_traverse_dir(dir, pt);
        kfree(pt);
    }

    kfree(arr);

    serial_printf("Inside dir: '%s'\n", dir->own_entry.name);

    return dir;
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
    if (fat_info->available_cluster_num != 0xFFFFFFFF)
        return fat_info->available_cluster_num;
    else {
        if (fat_info->free_cluster_count == 0) return 0;
        for (u32 i = 2; i < fat_info->free_cluster_count; i++) {
            // TODO: Look for 0x00000000
        }
        return 0;
    }
}

/*
NOTE: FAT32_WRITE'S BUFFER ARGUMENT, HAS TO BE AN ALLOCATED
U8 WITH A MULTIPLE OF 512 BYTES IN SIZE! OR ELSE IT COULD
POINT TO ANOTHER FILE (Because of how sectors works)
*/

// TODO: Fix writing inside dir
int fat32_write(const char* filename, u8* buffer, u32 size, u8 attributes) {
    fat32_directory* working_dir = fat_root_dir;
    for (int i = 0; i < strlen(filename); i++) {
        if (filename[i] == '/') {
            char* name = kmalloc(strlen(filename));
            memcpy(name, filename, strlen(filename));
            working_dir = fat32_find_subdir(name);
            kfree(name);
        }
    }

    for (u32 i = 0; i < working_dir->file_count; i++) {
        if ((u8)working_dir->entries[i].name[0] != 0xE5 &&
            (u8)working_dir->entries[i].name[0] != 0x0) {
                continue;
            }
        // Directory entry free!
        fat32_entry* entry = &working_dir->entries[i];

        char* fname = fat32_unprocess_name(filename);

        memcpy(entry->name, fname, 11);
        
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

        u8 root_sect_count = 1;
        if (working_dir->file_count * sizeof(fat32_entry) > 512) {
            root_sect_count = ALIGN_UP(working_dir->file_count * sizeof(fat32_entry), 512);
            root_sect_count /= 512;
        }

        entry->high_cluster_entry = (u16)(cluster >> 16);
        entry->low_cluster_entry = (u16)(cluster & 0xFFFF);

        entry->size = size;

        ata_write_multiple(working_dir->sector, root_sect_count, (u8*)working_dir->entries);
        ata_write_multiple(fat32_get_sector(cluster), sec_count, buffer);
        break;
    }
    return 0;
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

    printf("Reading from hehe/lmao.lol\n");

    u8* buffer = kmalloc(512);
    memset(buffer, 0, 512);
    fat32_read("hehe/lmao.lol", buffer);
    printf("'%s'\n", buffer);
    
    printf("Writing to hehe/test.txt\n");
    u8* buffer2 = kmalloc(512);
    char* str = "WRitten FILES!\n";
    memcpy(buffer2, str, strlen(str));
    fat32_write("hehe/test.txt", buffer2, 256, 0);
    
    printf("Reading from hehe/test.txt\n");
    memset(buffer, 0, 512);
    fat32_read("hehe/test.txt", buffer);
    printf("'%s'\n", buffer);
}
