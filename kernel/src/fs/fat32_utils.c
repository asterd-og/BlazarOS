#include <fs/fat32.h>

u32 fat32_get_sector(fat32_fs* fs, u32 cluster) {
    return ((cluster - 2) * fs->bpb->sectors_per_cluster) + (fs->partition_sector + fs->bpb->resv_sectors + (fs->bpb->fat_count * fs->ebpb->table_size_32));
}

u32 fat32_read_cluster_end(fat32_fs* fs, u32 cluster) {
    u32 cluster_size = fs->bpb->bytes_per_sector * fs->bpb->sectors_per_cluster;
    u8* table = (u8*)kmalloc(32 * 1024); // To make sure, we allocate the biggest cluster size!
    memset(table, 0, 32 * 1024);
    u32 fat_offset = cluster * 4;
    u32 fat_sector = fs->partition_sector + fs->bpb->resv_sectors + (fat_offset / cluster_size);
    u32 ent_offset = fat_offset % cluster_size;

    ata_read(fat_sector, table, fs->bpb->bytes_per_sector);

    u32 table_val = *(u32*)&table[ent_offset] & 0x0FFFFFFF;

    kfree(table);
    return table_val;
}

void fat32_populate_dir(fat32_fs* fs, fat32_directory* dir) {
    u32 next_sector = dir->sector;
    u32 next_cluster = dir->cluster;
    fat32_entry entries[16];
    int times_iterated = 0;
    dir->entries = (fat32_entry*)kmalloc(sizeof(fat32_entry) * 16);
    dir->file_count = 0;
    while (true) {
        ata_read(next_sector, &entries[0], 512);
        for (int i = 0; i < 16; i++) {
            if (entries[i].name[0] == 0)
                return;
            if (times_iterated > 0) {
                // We are iterating the second time!
                dir->entries = (fat32_entry*)krealloc(dir->entries, (sizeof(fat32_entry) * 16) * (times_iterated + 1));
            }
            memcpy(&dir->entries[(times_iterated * 16) + i], &entries[i], sizeof(fat32_entry));
            dir->file_count++;
        }
        times_iterated++;
        next_cluster = fat32_read_cluster_end(fs, next_cluster);
        next_sector = fat32_get_sector(fs, next_cluster);
    }
}

fat32_entry* fat32_get_entry(fat32_fs* fs, fat32_directory* dir, const char* name) {
    for (int i = 0; i < dir->file_count; i++)
        if (!memcmp(dir->entries[i].name, name, strlen(name))) {
            return &dir->entries[i];
        }
    return NULL;
}