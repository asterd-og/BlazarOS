#pragma once

#include <types.h>
#include <limine.h>

#include <lib/string/string.h>

#include <mm/pmm/pmm.h>
#include <mm/memory.h>

typedef uptr page_map;

extern page_map* vmm_kernel_pm;

extern symbol text_start_ld;
extern symbol text_end_ld;

extern symbol rodata_start_ld;
extern symbol rodata_end_ld;

extern symbol data_start_ld;
extern symbol data_end_ld;

void vmm_init();

page_map* vmm_new_pm();
void vmm_destroy_pm(page_map* pm);

void vmm_switch_pm(page_map* pm);

void vmm_map(page_map* pm, uptr vaddr, uptr paddr, u64 flags);
void vmm_unmap(page_map* pm, uptr vaddr);

void* vmm_alloc(page_map* pm, u64 pages);
void vmm_free(page_map* pm, void* ptr, u64 pages);