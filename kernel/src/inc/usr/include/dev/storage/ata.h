#pragma once

#include <types.h>

#include <arch/io.h>
#include <arch/idt/idt.h>

#define ATA_PRIMARY 0x1F0
#define ATA_SECONDARY 0x170

#define ATA_PRIMARY_CTRL 0x3F6
#define ATA_SECONDARY_CTRL 0x376

#define ATA_MASTER 0xA0
#define ATA_SLAVE 0xB0

#define ATA_WAIT 0x00
#define ATA_IDENTIFY 0xEC
#define ATA_READ 0x20
#define ATA_WRITE 0x30

// I won't define the next two buses. Not needed

void ata_init();

void ata_read_one(u32 lba);
void ata_read_multiple(u32 lba, u8 sec_count, u8* buffer);

void ata_write_one(u32 lba, u8* buffer);
void ata_write_multiple(u32 lba, u8 sec_count, u8* buffer);
