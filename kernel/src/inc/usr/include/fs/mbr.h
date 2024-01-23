#pragma once

#include <types.h>

#include <dev/storage/ata.h>

#include <mm/heap/heap.h>

typedef struct {
    u8 boot_indicator;
    u8 starting_head;
    u8 starting_sector : 6;
    u16 starting_cylinder : 10;
    u8 sys_id;
    u8 ending_head;
    u8 ending_sector : 6;
    u16 ending_cylinder : 10;
    u32 relative_sector; // Partition's starting LBA value
    u32 total_sectors;
} __attribute__((packed)) partition_info;

typedef struct {
    char bootstrap[440];
    u32 disk_id;
    u16 resv;
    partition_info partitions[4];
    char boot_signature[2];
} __attribute__((packed)) mbr_info;

void mbr_init();