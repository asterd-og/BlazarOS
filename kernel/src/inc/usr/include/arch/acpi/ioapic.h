#pragma once

#include <types.h>

#include <arch/acpi/madt.h>
#include <arch/acpi/lapic.h>

#include <mm/memory.h>

#define IOAPIC_REGSEL 0x0
#define IOAPIC_IOWIN  0x10

#define IOAPIC_ID     0x0
#define IOAPIC_VER    0x01
#define IOAPIC_ARB    0x02
#define IOAPIC_REDTBL 0x10

void ioapic_init();

void ioapic_redirect_irq(u32 lapic_id, u8 irq, bool mask);