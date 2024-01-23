#include <fs/fat32.h>
#include <mm/heap/heap.h>

fat32_fs* fats[256];
int fat_idx = 0;

u32 fat32_get_sector(fat32_fs* fs, u32 cluster) {
    return ((cluster - 2) * fs->bpb->sectors_per_cluster) + (fs->partition_sector + fs->bpb->resv_sectors + (fs->bpb->fat_count * fs->ebpb->table_size_32));
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

    printf("Partition %d:\n", fat_idx);

    fat32_entry entries[4];
    ata_read(fat32_get_sector(fs, ebpb->root_cluster), (u8*)&entries[0], sizeof(fat32_entry) * 4);

    for (u32 i = 0; i < 4; i++) {
        if (entries[i].name[0] == 0) break;
        if (entries[i].attributes & FAT_ATTR_HIDDEN) continue;
        printf("%s\n", entries[i].name);
    }

    fats[fat_idx] = fs;
    fat_idx++;
}