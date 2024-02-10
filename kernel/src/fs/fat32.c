#include <fs/fat32.h>
#include <mm/heap/heap.h>

// For some reason my fat32 doesn't works with KASLR! lmao...

fat32_directory* fat32_get_dir(fat32_fs* fs, const char* path) {
    fat32_directory* root_dir = hashmap_get(fs->cache_hashmap, path);
    if (root_dir != NULL) /* Directory has been hashed/cached already, return that.*/
        return root_dir;
    root_dir = (fat32_directory*)kmalloc(sizeof(fat32_directory));
    fat32_entry* entry = fat32_get_entry(fs, fs->root_dir, path);
    memset(root_dir, 0, sizeof(fat32_directory));
    if (entry == NULL || root_dir == NULL)
        return NULL;
    root_dir->cluster = ((u32)entry->high_cluster_entry << 16)
                         | ((u32)entry->low_cluster_entry);
    root_dir->sector = fat32_get_sector(fs, root_dir->cluster);
    fat32_populate_dir(fs, root_dir);
    hashmap_push(fs->cache_hashmap, path, root_dir);
    return root_dir;
}

int fat32_read(fat32_fs* fs, fat32_directory* dir, const char* filename, u8* buffer) {
    fat32_entry* entry = fat32_get_entry(fs, dir, filename);
    if (entry == NULL) return 1;

    u32 cluster = ((u32)entry->high_cluster_entry << 16)
                   | ((u32)entry->low_cluster_entry);
    
    ata_read(fat32_get_sector(fs, cluster), buffer, 512);

    if (entry->size > 512) {
        for (int i = 0; i < DIV_ROUND_UP(entry->size, 512); i++) {
            cluster = fat32_read_cluster_end(fs, cluster);
            ata_read(fat32_get_sector(fs, cluster), buffer + ((i + 1) * 512), 512);
        }
    }

    return 0;
}

// TODO: Handle if size is > 512 and then allocate more clusters for it!

int fat32_write(fat32_fs* fs, fat32_directory* dir, const char* filename, u8* buffer, u32 size, u16 attributes) {
    for (u32 i = 0; i < dir->file_count + 10; i++) {
        if ((u8)dir->entries[i].name[0] != 0xe5 && (u8)dir->entries[i].name[0] != 0x0) continue;
        fat32_entry* entry = &dir->entries[i];
        char* entry_name = fat32_unprocess_name(filename);
        memcpy(entry->name, entry_name, strlen(entry_name));
        kfree(entry_name);

        entry->attributes = attributes;
        u32 cluster = fat32_allocate_cluster(fs);

        entry->high_cluster_entry = (u16)(cluster >> 16);
        entry->low_cluster_entry = (u16)(cluster & 0xFFFF);

        entry->size = size;

        dir->file_count++;
        ata_write(fat32_get_sector(fs, cluster), buffer, size);
        fat32_flush_dir(fs, dir);
        return 0;
    }
    return 1;
}

fat32_fs* fat32_init(partition_info pt_info) {
    fat32_bpb* bpb = (fat32_bpb*)kmalloc(sizeof(fat32_bpb));
    ata_read(pt_info.relative_sector, bpb, sizeof(fat32_bpb));
    fat32_ebpb* ebpb = (fat32_ebpb*)kmalloc(sizeof(fat32_ebpb));
    memcpy(ebpb, bpb->extended_section, sizeof(fat32_ebpb));

    fat32_fs* fs = (fat32_fs*)kmalloc(sizeof(fat32_fs));
    fs->partition_sector = pt_info.relative_sector;
    fs->bpb = bpb;
    fs->ebpb = ebpb;
    fs->root_dir = (fat32_directory*)kmalloc(sizeof(fat32_directory));
    fs->root_dir->cluster = ebpb->root_cluster;
    fs->root_dir->sector = fat32_get_sector(fs, ebpb->root_cluster);
    fs->fat_info = (fat32_info*)kmalloc(sizeof(fat32_info));
    ata_read(ebpb->fsinfo_sector + pt_info.relative_sector, fs->fat_info, sizeof(fat32_info));

    fat32_populate_dir(fs, fs->root_dir);

    fs->cache_hashmap = hashmap_init(256, sizeof(fat32_directory), 25);

    return fs;
}