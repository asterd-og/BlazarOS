#include <fs/mbr.h>
#include <fs/fat32.h>

mbr_info* mbr;

void mbr_init() {
    mbr = (mbr_info*)kmalloc(sizeof(mbr_info));
    ata_read(0, mbr, sizeof(mbr_info));
    printf("%x\n", mbr->boot_signature[0]);

    for (int i = 0; i < 4; i++) {
        if (mbr->partitions[i].sys_id == 0xb || mbr->partitions[i].sys_id == 0x83) {
            // Fat32
            fat32_init(mbr->partitions[i]);
        }
    }
}