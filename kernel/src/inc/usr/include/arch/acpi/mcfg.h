#pragma once

#include <types.h>

typedef struct {
    u64 addr;
    u16 seg;
    u8 first_bus;
    u8 last_bus;
    u32 resv;
} __attribute__((packed)) mcfg_entry;

typedef struct {
    char sign[4];
    u32 len;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
    u64 resv;

    mcfg_entry table[];
}  __attribute__((packed)) acpi_mcfg;
