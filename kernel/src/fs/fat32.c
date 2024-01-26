#include <fs/fat32.h>
#include <mm/heap/heap.h>

fat32_fs* fats[256];
int fat_idx = 0;

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
        u32 cluster_chain_end = fat32_read_cluster_end(fs, next_cluster);
        next_cluster = cluster_chain_end;
        next_sector = fat32_get_sector(fs, next_cluster);
    }
}

int fat32_read(fat32_fs* fs, fat32_directory* dir, const char* filename, u8* buffer) {
    for (int i = 0; i < dir->file_count; i++) {
        if (!memcmp(dir->entries[i].name, filename, strlen(filename))) {
            fat32_entry entry = dir->entries[i];
            u32 cluster = ((u32)entry.high_cluster_entry << 16)
                           | ((u32)entry.low_cluster_entry);
            
            ata_read(fat32_get_sector(fs, cluster), buffer, 512);

            if (entry.size > 512) {
                for (int i = 0; i < DIV_ROUND_UP(entry.size, 512); i++) {
                    cluster = fat32_read_cluster_end(fs, cluster);
                    ata_read(fat32_get_sector(fs, cluster), buffer + ((i + 1) * 512), 512);
                }
            }

            return 0;
        }
    }
    return 1;
}

void fat32_init(partition_info pt_info) {
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

    fat32_populate_dir(fs, fs->root_dir);

    fats[fat_idx] = fs;
    fat_idx++;
}