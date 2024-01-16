#include <sys/elf.h>

u64 elf_load(char* elf, page_map* pm) {
    elf_header* hdr = (elf_header*)elf;

    if (hdr->magic[0] != 0x7f || hdr->magic[1] != 'E' ||
        hdr->magic[2] != 'L' || hdr->magic[3] != 'F')
            return -1;

    if (hdr->type != 2)
        return -1;
    
    elf_ph* phdr = (elf_ph*)(elf + hdr->phoff);

    for (u16 i = 0; i < hdr->entry_ph_count; i++, phdr++) {
        if (phdr->type == 1) {
            // Elf load
            u64 mem_size = DIV_ROUND_UP(phdr->mem_size, PAGE_SIZE);
            void* seg = HIGHER_HALF(pmm_alloc(mem_size));
            memcpy(seg, (void*)elf + phdr->offset, phdr->file_size);
            if (phdr->file_size < phdr->mem_size)
                memset(seg + phdr->file_size, 0, phdr->mem_size - phdr->file_size);
            vmm_map(pm, phdr->vaddr, (uptr)PHYSICAL(seg), PTE_PRESENT | PTE_WRITABLE | PTE_USER);
        }
    }
    return hdr->entry;
}