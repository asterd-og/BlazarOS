#pragma once

#include <types.h>

#include <lib/string/string.h>

#include <mm/vmm/vmm.h>
#include <mm/memory.h>

#define HEAP_MAGIC (u32)0xbadfacade

typedef struct {
    u32 magic;
    u64 pages;
} heap_memory_block;

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);