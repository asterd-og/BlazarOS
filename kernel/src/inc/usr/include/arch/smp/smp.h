#pragma once

#include <types.h>
#include <limine.h>

#include <arch/acpi/ioapic.h>
#include <arch/acpi/lapic.h>
#include <arch/idt/idt.h>
#include <arch/gdt/gdt.h>

#include <mm/pmm/pmm.h>
#include <mm/vmm/vmm.h>
#include <mm/memory.h>

#include <lib/log.h>

#include <sched/sched.h>

extern u32 bsp_lapic_id;
extern u64 smp_cpu_started;
extern u64 smp_cpu_count;

typedef struct {
    bool lock;
    u32 lapic_id;

    page_map* current_pm;

    process* current_proc;
    process* proc_list[256];
    u64 proc_size;
    u64 proc_idx;
    u64 proc_pr_count;
    u64 last_idle_time;
    u64 total_time;
} __attribute__((packed)) cpu_info;

void smp_init();

cpu_info* this_cpu();
cpu_info* get_cpu(u64 id);

u32 sched_get_cpu_load(cpu_info* cpu);