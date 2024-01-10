#pragma once

#include <types.h>

#include <dev/storage/ata.h>

typedef struct {
    u8 jmp[3];
    u8 oem[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 resv_sectors; // Boot sector included!
    u8 fat_count; // Often 2
    u16 root_dir_count;
    u16 logic_sectors_count;
    u8 media_desc_type;
    u16 fat16_sect_count;
    u16 sectors_per_track;
    u16 heads_sides_count;
    u32 hidden_sectors_count;
    u32 large_sectors_count; // Fat32 sectors count
} __attribute__((packed)) fat32_bpb;

typedef struct {
    u32 sectors_per_fat; // The size of FAT in sectors
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
    u8 boot_code[420];
    u16 boot_signature;
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

void fat32_init();
