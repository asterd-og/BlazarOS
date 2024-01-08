#pragma once

#include <types.h>

#include <arch/acpi/acpi.h>
#include <arch/acpi/madt.h>
#include <arch/cpu.h>

#include <mm/vmm/vmm.h>
#include <mm/memory.h>

#include <lib/log.h>

#define LAPIC_PPR 0x00a0

#define LAPIC_ICRLO 0x0300
#define LAPIC_ICRHI 0x0310

#define LAPIC_ICINI 0x0500
#define LAPIC_ICSTR 0x0600

#define LAPIC_ICEDGE 0x0000

#define LAPIC_ICPEND 0x00001000
#define LAPIC_ICPHYS 0x00000000
#define LAPIC_ICASSR 0x00004000
#define LAPIC_ICSHRTHND 0x00000000
#define LAPIC_ICDESTSHIFT 24

#define LAPIC_ICRAIS 0x00080000
#define LAPIC_ICRAES 0x000c0000

void lapic_init();

void lapic_write(u32 reg, u32 val);
u32 lapic_read(u32 reg);

void lapic_eoi();

void lapic_ipi(u32 id, u32 dat);
void lapic_send_all_int(u32 id, u32 vec);
void lapic_init_cpu(u32 id);
void lapic_start_cpu(u32 id, u32 vec);
u32 lapic_get_id();