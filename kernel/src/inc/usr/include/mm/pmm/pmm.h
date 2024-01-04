#pragma once

#include <types.h>
#include <limine.h>

#include <lib/string/string.h>

#include <mm/pmm/bitmap.h>
#include <mm/memory.h>

void pmm_init();
void* pmm_alloc(u64 pages);
void pmm_free(void* ptr, u64 pages);