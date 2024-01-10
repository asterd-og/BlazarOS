#include <fs/fat32.h>
#include <mm/heap/heap.h>

void fat32_init() {
    u8 mem[512];
    ata_read_multiple(0, 1, mem);
    fat32_bpb* bpb = (fat32_bpb*)mem;
    fat32_ebpb* ebpb = (fat32_ebpb*)(mem + 36);

    log_info("Fat bytes per sector: %d | sectors per cluster: %d | fat count: %d | fat32 sectors count: %d\n", bpb->bytes_per_sector,
            bpb->sectors_per_cluster, bpb->fat_count, bpb->total_sectors_32);
        
    
    log_info("Fat version: %d\n", ebpb->fat_version);
}
