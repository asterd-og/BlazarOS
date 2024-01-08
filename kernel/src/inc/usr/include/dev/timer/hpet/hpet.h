#pragma once

#include <types.h>

#include <arch/acpi/acpi.h>
#include <arch/idt/idt.h>

#include <mm/memory.h>

#include <dev/timer/pit/pit.h>

#define HPET_CONFIG 0x10
#define HPET_COUNTER 0xf0

#define HPET_CAP_LEG 1 << 15
#define HPET_CAP_64 1 << 13

typedef struct {
    u8 addr_space_id;
    u8 regs_width;
    u8 regs_offset;
    u8 resv;
    u64 address;
} __attribute__((packed)) hpet_address;

typedef struct {
    acpi_sdt hdr;
    u8 hw_rev_id;
    u8 comparator_count : 5;
    u8 counter_size : 1;
    u8 resv : 1;
    u8 legacy : 1;
    u16 pci_id;
    hpet_address addr_info;
    u8 hpet_num;
    u16 min_tick;
    u8 page_prot;
} __attribute__((packed)) hpet_info;

void hpet_init();