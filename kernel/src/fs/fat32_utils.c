#include <fs/fat32.h>

// PATH FUNCTIONS

char* fat32_normalise_name(const char* name) {
    char* res = (char*)kmalloc(11);
    memset(res, 0, 11);
    int i = 0;
    int j = 0;
    for (; i < 8; i++) {
        if (name[i] == ' ') break;
        res[i] = name[i];
    }
    if (name[10] == ' ' && name[9] == ' ' && name[8] == ' ') return res;
    res[i] = '.';
    i++;
    for(j = 0; j < 3; j++) {
        if (name[8 + j] == 0 || name[8 + j] == ' ') return res;
        res[i + j] = name[8 + j];
    }
    return res;
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

void fat32_write_cluster_end(fat32_fs* fs, u32 cluster, u32 val) {
    u32 cluster_size = fs->bpb->bytes_per_sector * fs->bpb->sectors_per_cluster;
    u8* table = (u8*)kmalloc(32 * 1024); // To make sure, we allocate the biggest cluster size!
    memset(table, 0, 32 * 1024);
    u32 fat_offset = cluster * 4;
    u32 fat_sector = fs->partition_sector + fs->bpb->resv_sectors + (fat_offset / cluster_size);
    u32 ent_offset = fat_offset % cluster_size;

    ata_read(fat_sector, table, fs->bpb->bytes_per_sector);

    *(u32*)&table[ent_offset] = val;

    ata_write(fat_sector, table, fs->bpb->bytes_per_sector);

    kfree(table);
}

void fat32_populate_dir(fat32_fs* fs, fat32_directory* dir) {
    u32 next_sector = dir->sector;
    u32 next_cluster = dir->cluster;
    fat32_entry entries[16];
    int times_iterated = 0;
    dir->entries = (fat32_entry*)kmalloc(sizeof(fat32_entry) * 16);
    dir->file_count = 0;
    while (true) {
        ata_read(next_sector, (u8*)&entries[0], 512);
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

u32 fat32_allocate_cluster(fat32_fs* fs) {
    if (fs->fat_info->available_cluster_num != 0xFFFFFFFF) {
        u32 ret = fs->fat_info->available_cluster_num;
        u32 sector = fat32_get_sector(fs, ret);
        // Find next available cluster
        u8* cluster_buf = kmalloc(512);

        memset(cluster_buf, 0, 512);
        ata_read(sector, cluster_buf, 512);
        
        if (cluster_buf[0] != 0x00000000) {
            fs->fat_info->available_cluster_num++;
            while (cluster_buf[0] != 0x00000000) {
                sector = fat32_get_sector(fs, fs->fat_info->available_cluster_num);
                ata_read(sector, cluster_buf, 512);
                fs->fat_info->available_cluster_num++;
                ret++;
            }
            // Found free cluster!
        } else {
            fs->fat_info->available_cluster_num++;
        }

        kfree(cluster_buf);
        ata_write(fs->ebpb->fsinfo_sector, (u8*)fs->fat_info, 512);

        return ret;
    } else {
        if (fs->fat_info->free_cluster_count == 0) return 0;
        for (u32 i = 2; i < fs->fat_info->free_cluster_count; i++) {
            // TODO: Look for 0x00000000 beginning at cluster 2
        }
        serial_printf("FAT32: Couldn't find free cluster!\n");
        return 0;
    }
}

u32 fat32_get_new_cluster_chain(fat32_fs* fs, u32 cluster_num) {
    u32 first_cluster = fat32_allocate_cluster(fs);
    for (u32 i = 0; i < cluster_num - 1; i++) {
        if (fat32_allocate_cluster(fs) == 0) {
            return 0;
        }
    }
    return first_cluster;
}

void fat32_flush_dir(fat32_directory* working_dir) {
    u32 root_sect_count = DIV_ROUND_UP(working_dir->file_count * sizeof(fat32_entry), 512);

    u8* copy_buf = kmalloc(root_sect_count * 512);
    memcpy(copy_buf, working_dir->entries, (working_dir->file_count * sizeof(fat32_entry)));

    ata_write(working_dir->sector, copy_buf, root_sect_count * 512);

    kfree(copy_buf);
}

fat32_entry* fat32_get_entry(fat32_directory* dir, const char* name) {
    char* fname = fat32_unprocess_name(name);
    for (u32 i = 0; i < dir->file_count; i++) {
        if (!memcmp(dir->entries[i].name, name, 11)) {
            kfree(fname);
            return &dir->entries[i];
        }
    }
    kfree(fname);
    return NULL;
}