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

void smp_init();