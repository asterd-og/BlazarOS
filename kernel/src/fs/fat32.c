#include <fs/fat32.h>
#include <mm/heap/heap.h>

void fat32_init() {
    u8 mem[512];
    ata_read_one(0, mem);
    fat32_bpb* bpb = (fat32_bpb*)mem;

    log_info("Fat bytes per sector: %d | sectors per cluster: %d | fat count: %d | fat32 sectors count: %d\n", bpb->bytes_per_sector,
            bpb->sectors_per_cluster, bpb->fat_count, bpb->large_sectors_count);
}
