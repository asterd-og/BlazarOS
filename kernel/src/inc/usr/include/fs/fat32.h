#pragma once

#include <types.h>

#include <dev/storage/ata.h>
#include <dev/serial/serial.h>

#include <lib/hashmap.h>

#include <fs/mbr.h>

#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_LFN 0x01 | 0x02 | 0x04 | 0x08 // long file name

#define FAT_END_CLUSTER 0xFFFFFFF

typedef struct {
    u8 jmp[3];
    u8 oem[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 resv_sectors; // Boot sector included!
    u8 fat_count; // Often 2
    u16 root_dir_count;
    u16 total_sectors_16;
    u8 media_desc_type;
    u16 table_size_16;
    u16 sectors_per_track;
    u16 heads_sides_count;
    u32 hidden_sectors_count;
    u32 total_sectors_32; // Fat32 sectors count

    // Microsoft docs says that this will be casted into the EBPB
    u8 extended_section[54];
} __attribute__((packed)) fat32_bpb;

typedef struct {
    u32 table_size_32; // The size of FAT in sectors
    u16 flags;
    u16 fat_version; // High byte = major version | Low byte = minor version
    u32 root_cluster; // Often 2
    u16 fsinfo_sector;
    u16 bbs_sector; // bbs = backup boot sector
    char resv[12];
    u8 drive_num;
    u8 resv1;
    u8 signature; // Must be 0x28 or 0x29
    u32 volume_id;
    char volume_label[11];
    char system_id[8];
} __attribute__((packed)) fat32_ebpb;

typedef struct {
    u32 lead_signature;
    u8 resv[480];
    u32 signature;
    u32 free_cluster_count;
    u32 available_cluster_num;
    u8 resv1[12];
    u32 trail_signature;
} __attribute__((packed)) fat32_info;

typedef struct {
    char name[11];

    u8 attributes;
    u8 resv;
    
    u8 creation_time_seconds;
    u16 creation_time;
    u16 creation_date;
    
    u16 last_accessed_date;
    
    u16 high_cluster_entry;
    
    u16 last_mod_time;
    u16 last_mod_date;
    
    u16 low_cluster_entry;

    u32 size;
} __attribute__((packed)) fat32_entry;

struct fat32_directory {
    struct fat32_directory* parent;
    fat32_entry* own_entry;
    fat32_entry* entries;
    u32 file_count;
    u32 sector;
    u32 cluster;
} __attribute__((packed));

typedef struct fat32_directory fat32_directory;

typedef struct {
    fat32_bpb* bpb;
    fat32_ebpb* ebpb;
    fat32_directory* root_dir;
    u32 partition_sector;
    hashmap_table* cache_hashmap;
    fat32_info* fat_info;
} __attribute__((packed)) fat32_fs;

fat32_fs* fat32_init(partition_info pt_info);
fat32_fs* fat32_get_partition(u32 pt_num);

int fat32_read(fat32_fs* fs, fat32_directory* dir, const char* filename, u8* buffer);
int fat32_write(fat32_fs* fs, fat32_directory* dir, const char* filename, u8* buffer, u32 size, u16 attr);

u32 fat32_get_sector(fat32_fs* fs, u32 cluster);
u32 fat32_read_cluster_end(fat32_fs* fs, u32 cluster);
void fat32_populate_dir(fat32_fs* fs, fat32_directory* dir);
fat32_entry* fat32_get_entry(fat32_fs* fs, fat32_directory* dir, const char* name);