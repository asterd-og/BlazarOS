#include <arch/acpi/madt.h>

madt_ioapic* madt_ioapic_list[128] = {0};
madt_iso* madt_iso_list[128] = {0};

u32 madt_ioapic_len;
u32 madt_iso_len;

u64* lapic_addr;

void madt_init() {
    acpi_madt* madt = (acpi_madt*)acpi_find_table("APIC");
    log_info("ACPI: MADT Found at %lx.\n", madt);

    u64 off = 0;

    int current_idx = 0;
    madt_ioapic_len = 0;
    madt_iso_len = 0;

    while (true) {
        if (off > madt->len - sizeof(madt))
            break;
        
        madt_entry* entry = (madt_entry*)(madt->table + off);

        switch (entry->type) {
            case 0:
                log_info("MADT: Found CPU LAPIC %d.\n", current_idx);
                current_idx++;
                break;
            case 1:
                log_info("MADT: Found IOAPIC address = %x.\n", ((madt_ioapic*)entry)->apic_addr);
                madt_ioapic_list[madt_ioapic_len++] = (madt_ioapic*)entry;
                break;
            case 2:
                log_info("MADT: Found ISO bus src = %x | irq src = %x.\n", ((madt_iso*)entry)->bus_src, ((madt_iso*)entry)->irq_src);
                madt_iso_list[madt_iso_len++] = (madt_iso*)entry;
                break;
            case 5:
                log_info("MADT: Found LAPIC address = %lx.\n", ((madt_lapic_addr*)entry)->phys_lapic);
                lapic_addr = (u64*)((madt_lapic_addr*)entry)->phys_lapic;
                break;
        }

        off += entry->len;
    }
}