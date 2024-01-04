#include <mm/vmm/vmm.h>

volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

page_map* vmm_current_pm;
page_map* vmm_kernel_pm;

uptr* vmm_get_next_level(uptr* level, uptr entry, u64 flags, bool alloc) {
    if (level[entry] & PTE_PRESENT) {
        return HIGHER_HALF(PTE_GET_ADDR(level[entry]));
    }

    if (alloc) {
        uptr* pml = (uptr*)HIGHER_HALF(pmm_alloc(1));
        memset(pml, 0, PAGE_SIZE);
        level[entry] = (uptr)PHYSICAL(pml) | flags;
        return pml;
    }

    return NULL;
}

page_map* vmm_new_pm() {
    page_map* pm = (page_map*)HIGHER_HALF(pmm_alloc(1));
    memset(pm, 0, PAGE_SIZE);

    // Copy kernel's higher half mappings to the new pm
    for (usize i = 256; i < 512; i++) {
        pm[i] = vmm_kernel_pm[i];
    }

    return pm;
}

void vmm_destroy_pm(page_map* pm) {
    if (vmm_current_pm == pm) {
        vmm_switch_pm(vmm_kernel_pm);
    }

    memset(pm, 0, PAGE_SIZE);
    pmm_free(PHYSICAL(pm), 1);
}

void vmm_switch_pm(page_map* pm) {
    write_cr3((u64)PHYSICAL(pm));
    vmm_current_pm = pm;
}

void vmm_map(page_map* pm, uptr vaddr, uptr paddr, u64 flags) {
    uptr pml1_entry = (vaddr >> 12) & 0x1ff;
    uptr pml2_entry = (vaddr >> 21) & 0x1ff;
    uptr pml3_entry = (vaddr >> 30) & 0x1ff;
    uptr pml4_entry = (vaddr >> 39) & 0x1ff;

    uptr* pml3 = vmm_get_next_level(pm, pml4_entry, PTE_PRESENT | PTE_WRITABLE, true); // pml4[pml4Entry] = pml3
    uptr* pml2 = vmm_get_next_level(pml3, pml3_entry, PTE_PRESENT | PTE_WRITABLE, true);     // pml3[pml3Entry] = pml2
    uptr* pml1 = vmm_get_next_level(pml2, pml2_entry, PTE_PRESENT | PTE_WRITABLE, true);     // pml2[pml2Entry] = pml1

    pml1[pml1_entry] = paddr | flags;
}

void vmm_unmap(page_map* pm, uptr vaddr) {
    uptr pml1_entry = (vaddr >> 12) & 0x1ff;
    uptr pml2_entry = (vaddr >> 21) & 0x1ff;
    uptr pml3_entry = (vaddr >> 30) & 0x1ff;
    uptr pml4_entry = (vaddr >> 39) & 0x1ff; // page_map

    uptr* pml3 = vmm_get_next_level(pm, pml4_entry, PTE_PRESENT | PTE_WRITABLE, false); // pml4[pml4Entry] = pml3
    if (pml3 == NULL) {
        return;
    }
    uptr* pml2 = vmm_get_next_level(pml3, pml3_entry, PTE_PRESENT | PTE_WRITABLE, false);    // pml3[pml3Entry] = pml2
    if (pml2 == NULL) {
        return;
    }
    uptr* pml1 = vmm_get_next_level(pml2, pml2_entry, PTE_PRESENT | PTE_WRITABLE, false);    // pml2[pml2Entry] = pml1
    if (pml1 == NULL) {
        return;
    }

    // Flush page
    pml1[pml1_entry] = 0;
    __asm__ volatile("invlpg (%0)" : : "b"(vaddr) : "memory");
}

void* vmm_alloc(page_map* pm, u64 pages) {
    uptr addr = (uptr)pmm_alloc(pages);

    uptr virt;
    uptr phys;

    for (u64 i = 0; i < pages; i++) {
        virt = addr + (i * PAGE_SIZE);
        phys = addr + (i * PAGE_SIZE);
        vmm_map(pm, virt, phys, PTE_PRESENT | PTE_WRITABLE);
    }

    return (void*)addr;
}

void vmm_free(page_map* pm, void* ptr, u64 pages) {
    uptr virt;
    
    for (u64 i = 0; i < pages; i++) {
        virt = (uptr)ptr + (i * PAGE_SIZE);
        vmm_unmap(pm, virt);
    }

    pmm_free(ptr, pages);
}

void vmm_init() {
    vmm_kernel_pm = (page_map*)HIGHER_HALF(pmm_alloc(1));
    memset(vmm_kernel_pm, 0, PAGE_SIZE);

    uptr phys_base = kernel_address_request.response->physical_base;
    uptr virt_base = kernel_address_request.response->virtual_base;

    uptr text_start = ALIGN_DOWN((uptr)text_start_ld, PAGE_SIZE);
    uptr text_end = ALIGN_UP((uptr)text_end_ld, PAGE_SIZE);

    uptr rodata_start = ALIGN_DOWN((uptr)rodata_start_ld, PAGE_SIZE);
    uptr rodata_end = ALIGN_UP((uptr)rodata_end_ld, PAGE_SIZE);

    uptr data_start = ALIGN_DOWN((uptr)data_start_ld, PAGE_SIZE);
    uptr data_end = ALIGN_UP((uptr)data_end_ld, PAGE_SIZE);

    for (uptr text = text_start; text < text_end; text += PAGE_SIZE)
        vmm_map(vmm_kernel_pm, text, text - virt_base + phys_base, PTE_PRESENT);

    for (uptr rodata = rodata_start; rodata < rodata_end; rodata += PAGE_SIZE)
        vmm_map(vmm_kernel_pm, rodata, rodata - virt_base + phys_base, PTE_PRESENT | PTE_NX);

    for (uptr data = data_start; data < data_end; data += PAGE_SIZE)
        vmm_map(vmm_kernel_pm, data, data - virt_base + phys_base, PTE_PRESENT | PTE_WRITABLE | PTE_NX);

    for (uptr gb4 = 0; gb4 < 0x100000000; gb4 += PAGE_SIZE) {
        vmm_map(vmm_kernel_pm, gb4, gb4, PTE_PRESENT | PTE_WRITABLE);
        vmm_map(vmm_kernel_pm, (uptr)HIGHER_HALF(gb4), gb4, PTE_PRESENT | PTE_WRITABLE);
    }

    vmm_switch_pm(vmm_kernel_pm);
}