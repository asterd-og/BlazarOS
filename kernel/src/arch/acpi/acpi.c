#include <arch/acpi/acpi.h>

struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

void* acpi_root_rsdt;

void* acpi_find_table(const char* name) {
    acpi_rsdt* rsdt = (acpi_rsdt*)acpi_root_rsdt;
    u32 entries = (rsdt->rsdt.len - sizeof(rsdt->rsdt)) / 4;

    for (u32 i = 0; i < entries; i++) {
        acpi_sdt* sdt = (acpi_sdt*)((u64)rsdt->table[i]);
        if (!memcmp(sdt->sign, name, 4))
            return (void*)sdt;
    }

    return NULL;
}

u32 acpi_init() {
    acpi_rsdp* rsdp = (acpi_rsdp*)rsdp_request.response->address;

    if (rsdp->revision != 0)
        return -1;

    if (memcmp(rsdp->sign, "RSD PTR", 7))
        return -2;
    
    acpi_root_rsdt = (void*)(u64)rsdp->rsdt_addr;

    return rsdp->rsdt_addr;
}