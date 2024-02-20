#pragma once

#include <types.h>
#include <limine.h>

#include <lib/string/string.h>

#include <mm/pmm/bitmap.h>
#include <mm/memory.h>

extern u64 pmm_total_memory;
extern u64 pmm_free_memory;
extern u64 pmm_used_memory;

void pmm_init();
void* pmm_alloc(u64 pages);
void pmm_free(void* ptr, u64 pages);