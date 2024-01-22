#include <fs/fat32.h>
#include <mm/heap/heap.h>

void fat32_init() {
    fat32_bpb* fat_bpb = (fat32_bpb*)kmalloc(sizeof(fat32_bpb));
    ata_read(0, fat_bpb, sizeof(fat32_bpb));
}