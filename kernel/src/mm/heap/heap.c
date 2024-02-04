#include <mm/heap/heap.h>

void* kmalloc(size_t size) {
    // Everything allocated in higher half
    // Is shared between all page maps
    u64 pages = ALIGN_UP(size + sizeof(heap_memory_block), PAGE_SIZE) / PAGE_SIZE;
    void* ptr = HIGHER_HALF(vmm_alloc(vmm_kernel_pm, pages));
    heap_memory_block* memory_block = (heap_memory_block*)ptr;
    memory_block->magic = HEAP_MAGIC;
    memory_block->pages = pages;
    return (ptr + sizeof(heap_memory_block));
}

void* krealloc(void* ptr, size_t size) {
    void* new_ptr = kmalloc(size);
    if (ptr == NULL) return new_ptr;
    void* obj = (ptr - sizeof(heap_memory_block));
    heap_memory_block* memory_block = (heap_memory_block*)obj;
    memcpy(new_ptr, ptr, memory_block->pages * PAGE_SIZE);
    kfree(ptr);
    return new_ptr;
}

void kfree(void* ptr) {
    if (ptr == NULL) return;
    void* obj = (ptr - sizeof(heap_memory_block));
    heap_memory_block* memory_block = (heap_memory_block*)obj;
    memset(obj, 0, memory_block->pages * PAGE_SIZE);
    vmm_free(vmm_kernel_pm, PHYSICAL(obj), memory_block->pages);
}

void* malloc(size_t size) {
    // This one is for the current pagemap instead of shared
    u64 pages = ALIGN_UP(size + sizeof(heap_memory_block), PAGE_SIZE) / PAGE_SIZE;
    void* ptr = HIGHER_HALF(vmm_alloc(vmm_current_pm, pages));
    heap_memory_block* memory_block = (heap_memory_block*)ptr;
    memory_block->magic = HEAP_MAGIC;
    memory_block->pages = pages;
    return (ptr + sizeof(heap_memory_block));
}

void* realloc(void* ptr, size_t size) {
    void* new_ptr = malloc(size);
    if (ptr == NULL) return new_ptr;
    void* obj = (ptr - sizeof(heap_memory_block));
    heap_memory_block* memory_block = (heap_memory_block*)obj;
    memcpy(new_ptr, ptr, memory_block->pages * PAGE_SIZE);
    free(ptr);
    return new_ptr;
}

void free(void* ptr) {
    if (ptr == NULL) return;
    void* obj = (ptr - sizeof(heap_memory_block));
    heap_memory_block* memory_block = (heap_memory_block*)obj;
    memset(obj, 0, memory_block->pages * PAGE_SIZE);
    vmm_free(vmm_current_pm, PHYSICAL(obj), memory_block->pages);
}