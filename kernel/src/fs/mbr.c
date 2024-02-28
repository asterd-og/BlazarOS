#include <fs/mbr.h>
#include <fs/fat32.h>

mbr_info* mbr;

fat32_fs* fats;
u32 pt_idx;

void mbr_init() {
    mbr = (mbr_info*)kmalloc(sizeof(mbr_info));
    ata_read(0, (u8*)mbr, sizeof(mbr_info));
    fats = (fat32_fs*)kmalloc(sizeof(fat32_fs) * 5);
    memset(fats, 0, sizeof(fat32_fs) * 5);

    for (pt_idx = 0; pt_idx < 4; pt_idx++) {
        if (mbr->partitions[pt_idx].sys_id == 0xb || mbr->partitions[pt_idx].sys_id == 0x83) {
            // Fat32
            memcpy(&fats[pt_idx], fat32_init(mbr->partitions[pt_idx]), sizeof(fat32_fs));
        }
    }
}

fat32_fs* fat32_get_partition(u32 pt_num) {
    if (pt_num > pt_idx) return NULL;
    return &fats[pt_num];
}