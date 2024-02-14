#include <mm/pmm/pmm.h>

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

u8* pmm_bitmap;
u64 pmm_bitmap_size;
u64 pmm_bitmap_index = 0;

u64 pmm_pages;
u64 pmm_free_memory;

void* pmm_alloc(u64 pages) {
    u64 pages_free = 0;
    bool looped = false;

    while (true) {
        if (pmm_bitmap_index > pmm_pages) {
            if (looped)
                return NULL;
            looped = true;
            pmm_bitmap_index = 0;
            // Re-search the entire list, maybe we freed something.
        } else if (pages_free == pages) {
            // Found it.
            break;
        } else if (bitmap_get(pmm_bitmap, pmm_bitmap_index) == 0) {
            pages_free++;
            pmm_bitmap_index++;
        } else {
            pages_free = 0;
            pmm_bitmap_index++;
        }
    }

    for (u64 i = 0; i < pages; i++) {
        bitmap_set(pmm_bitmap, (pmm_bitmap_index - pages) + i);
    }

    return (void*)((pmm_bitmap_index - pages) * PAGE_SIZE);
}

void pmm_free(void* ptr, u64 pages) {
    u64 page_index = (u64)ptr;
    page_index /= PAGE_SIZE;

    for (u64 i = 0; i < pages; i++) {
        bitmap_clear(pmm_bitmap, page_index + i);
    }

    pmm_bitmap_index = page_index;

    return;
}

void pmm_init() {
    u64 higher_address = 0;
    u64 top_address = 0;

    struct limine_memmap_response* memmap_response = 
        memmap_request.response;
    
    struct limine_memmap_entry* memmap_entry;

    for (u64 entry = 0; entry < memmap_response->entry_count; entry++) {
        memmap_entry = memmap_response->entries[entry];
        if (memmap_entry->type != LIMINE_MEMMAP_USABLE)
            continue;
        top_address = memmap_entry->base + memmap_entry->length;
        if (top_address > higher_address)
            higher_address = top_address;
    }

    pmm_free_memory = higher_address;
    pmm_pages = pmm_free_memory / PAGE_SIZE;
    pmm_bitmap_size = ALIGN_UP(pmm_pages / 8, PAGE_SIZE);

    for (u64 entry = 0; entry < memmap_response->entry_count; entry++) {
        memmap_entry = memmap_response->entries[entry];
        if (memmap_entry->type != LIMINE_MEMMAP_USABLE)
            continue;
        if (memmap_entry->length < pmm_bitmap_size)
            continue;

        pmm_bitmap = (u8*)HIGHER_HALF(memmap_entry->base);
        memset(pmm_bitmap, 0xFF, pmm_bitmap_size);
        
        memmap_entry->base += pmm_bitmap_size;
        memmap_entry->length -= pmm_bitmap_size;
        break;
    }

    for (u64 entry = 0; entry < memmap_response->entry_count; entry++) {
        memmap_entry = memmap_response->entries[entry];
        if (memmap_entry->type != LIMINE_MEMMAP_USABLE)
            continue;
        
        for (u64 i = 0; i < memmap_entry->length; i += PAGE_SIZE) {
            bitmap_clear(pmm_bitmap, (memmap_entry->base + i) / PAGE_SIZE);
        }
    }
}