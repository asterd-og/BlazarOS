#include <fs/fat32.h>
#include <mm/heap/heap.h>

u32 fat_total_clusters = 0;
u32 fat_first_sector = 0;
u32 fat_data_sector = 0;
u32 fat_root_sector = 0;
u32 fat_file_count = 0;

fat32_bpb* fat_bpb;
fat32_ebpb* fat_ebpb;

fat32_dir* fat_entries;

u32 fat32_get_sector(u32 cluster) {
    return fat_data_sector + fat_bpb->sectors_per_cluster * (cluster - 2);
}

int fat32_read(const char* filename, u8* buffer) {
    char name[9];
    char ext[4];

    ext[3] = 0;

    u8 i = 0;
    for (; i < 8; i++) {
        if (filename[i] == '.') break;
        name[i] = filename[i];
    }
    name[i] = 0;
    i++;
    u8 j = 0;
    for (; j < 3; j++)
        ext[j] = filename[i + j];

    serial_printf("Filename: '%s' Ext: '%s'.\n", name, ext);

    for (u32 i = 0; i < fat_file_count; i++) {
        if (!memcmp(name, fat_entries[i].name, i - 1) &&
            !memcmp(ext, fat_entries[i].ext, j)) {
            if ((fat_entries[i].attributes & FAT_ATTR_DIRECTORY) == FAT_ATTR_DIRECTORY) return 2;
            u32 file_cluster = ((u32)fat_entries[i].high_cluster_entry << 16) | ((u32)fat_entries[i].low_cluster_entry);
            u32 file_sector = fat32_get_sector(file_cluster);

            if (fat_entries[i].size > 512) {
                u8 sec_count = ALIGN_UP(fat_entries[i].size, 512);
                sec_count /= 512;
                ata_read_multiple(file_sector, sec_count, buffer);
            } else
                ata_read_multiple(file_sector, 1, buffer);
            buffer[fat_entries[i].size] = 0;
            return 0;
        }
    }
    return 1;
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
    fat32_dir* dir;
    u32 loc = 0;
    u32 sector_count = 0;

    while (true) {
        dir = (fat32_dir*)(mem2 + (loc * sizeof(fat32_dir)));
        if (dir->name[0] == 0) break;
        fat_file_count++;
        loc++;
    }

    fat_entries = (fat32_dir*)kmalloc(sizeof(fat32_dir) * fat_file_count);

    u8* dir_buffer = kmalloc(512);
    ata_read_multiple(fat_root_sector, 1, dir_buffer);
    for (u32 i = 0; i < fat_file_count; i++) {
        memcpy(&fat_entries[i], dir_buffer + (i * sizeof(fat32_dir)), sizeof(fat32_dir));
    }

    u8 buffer[1024];
    int status = fat32_read("HEY.TXT", buffer);
    if (status == 1) {
        log_bad("Couldn't find HEY.TXT!\n");
    } else if (status == 2) {
        log_bad("Not a file!\n");
    } else {
        log_info("Found HEY.TXT: %s", buffer);
    }
}
