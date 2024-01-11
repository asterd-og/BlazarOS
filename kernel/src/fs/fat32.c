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

u32 fat32_get_sector(u32 cluster) {
    return ((cluster - 2) * fat_bpb->sectors_per_cluster) + fat_data_sector;
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
    char name[9];
    char ext[4];

    ext[3] = 0;

    u8 i = 0;
    for (; i < 8; i++) {
        if (filename[i] == '.') break;
        name[i] = filename[i];
    }
    name[i] = 0;
    u8 fname_len = i;
    i++;
    u8 j = 0;
    for (; j < 3; j++)
        ext[j] = filename[i + j];
 
    for (u32 i = 0; i < working_dir->file_count; i++) {
        if (!memcmp(name, working_dir->entries[i].name, fname_len) &&
            !memcmp(ext, working_dir->entries[i].ext, j)) {
            fat32_entry entry = working_dir->entries[i];
            if ((entry.attributes & FAT_ATTR_DIRECTORY) == FAT_ATTR_DIRECTORY) break;
            return entry;
        }
    }

    return (fat32_entry){0};
}

fat32_entry fat32_traverse_path(char* path) {
    int i = 0;
    int file_count = 0;
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

    for (u32 j = 0; j < i - 1; j++) {
        dir = fat32_traverse_dir(dir, arr[j]);
    }
    
    fat32_entry entry = fat32_get_entry(dir, arr[i - 1]);

    kfree(arr);

    return entry;
}

int fat32_read(const char* filename, u8* buffer) {
    fat32_directory* working_dir = fat_root_dir;
    fat32_entry entry;
    bool in_dir = false;
    
    for (u32 i = 0; i < strlen(filename); i++) {
        if (filename[i] == '/') {
            char* name = kmalloc(strlen(filename));
            memcpy(name, filename, strlen(filename));
            entry = fat32_traverse_path(name);
            kfree(name);
            in_dir = true;
        }
    }

    if (!in_dir) entry = fat32_get_entry(fat_root_dir, filename);

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

void fat32_init() {
    u8* mem = kmalloc(512);
    ata_read_multiple(0, 1, mem);
    fat32_bpb* bpb = (fat32_bpb*)mem;
    fat32_ebpb* ebpb = (fat32_ebpb*)bpb->extended_section;

    if (mem[510] != 0x55 || mem[511] != 0xAA)
        log_bad("FAT: Bad fat32\n");
    else
        log_ok("FAT: Detect fat32!\n");
    
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