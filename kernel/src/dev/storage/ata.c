#include <dev/storage/ata.h>
#include <mm/memory.h>

// For now I use polling, I do plan on changing it to IRQ later!

// We will define later for primary or secondary
u16 ata_io_base = 0;
u8 ata_type = 0;

void ata_primary_irq(registers* regs) {
    (void)regs;
    __asm__ ("nop");
}

void ata_secondary_irq(registers* regs) {
    (void)regs;
    __asm__ ("nop"); // Acknowledge secondary ATA
}

void ata_delay() {
    for (u8 i = 0; i < 4; i++)
        inb(ata_io_base + 7);
}

void ata_poll() {
    ata_delay();

    u8 status;
    for (;;) {
        status = inb(ata_io_base + 7);
        if (!(status & 0x80)) break;
        if (status & 0x08) break;
        else if (status & 0x01) {
            log_bad("ATA: Disk err!\n");
        }
    }
}

void ata_read(u32 lba, u8* buffer, u32 size) {
    outb(ata_io_base + 6, (ata_type == ATA_MASTER ? 0xE0 : 0xF0) | ((lba >> 24) & 0x0F)); // Set master/slave
    outb(ata_io_base + 1, ATA_WAIT); // Send wait
    outb(ata_io_base + 2, DIV_ROUND_UP(size, 512)); // Sector count
    outb(ata_io_base + 3, (u8)lba); // Start sending LBA
    outb(ata_io_base + 4, (u8)(lba >> 8));
    outb(ata_io_base + 5, (u8)(lba >> 16)); // 24-bit LBA addressing only for now
    outb(ata_io_base + 7, ATA_READ);

    u16 val = 0;

    ata_poll();

    for (u32 i = 0; i < size; i += 2) {
        val = inw(ata_io_base);
        buffer[i] = val & 0x00ff;
        if (i + 1 < size)
            buffer[i + 1] = (val >> 8) & 0x00ff;
    }

    ata_delay();
}

void ata_write(u32 lba, u8* buffer, u32 size) {
    outb(ata_io_base + 6, (ata_type == ATA_MASTER ? 0xE0 : 0xF0) | ((lba >> 24) & 0x0F));
    outb(ata_io_base + 1, ATA_WAIT);
    outb(ata_io_base + 2, DIV_ROUND_UP(size, 512));
    outb(ata_io_base + 3, (u8)lba);
    outb(ata_io_base + 4, (u8)(lba >> 8));
    outb(ata_io_base + 5, (u8)(lba >> 16));
    outb(ata_io_base + 7, ATA_WRITE);

    u16 val = 0;
    u32 i = 0;

    ata_poll();

    for (; i < size; i += 2) {
        val = buffer[i];
        if (i + 1 < size)
            val |= ((u16)buffer[i + 1] << 8);
        outw(ata_io_base, val);
    }

    for (; i < ALIGN_UP(size, 512); i++)
        outw(ata_io_base, 0);
    
    ata_delay();
}

int ata_identify(u16 ata, u8 type) {
    ata_io_base = ata;
    ata_type = type;
    // Selects disk
    outb(ata + 6, type);
    for (int i = 0x1F2; i != 0x1F5; i++)
        outb(i, 0); // OSDev says to set those ports to 0
    outb(ata + 7, ATA_IDENTIFY);

    ata_delay();

    u8 status = inb(ata + 7);

    if (status == 0) return 1;
    
    for (;;) {
        status = inb(ata + 7);
        if (!(status & 0x80)) break;
        if (status & 0x08) break;
        if (status & 0x01) return 2;
    }

    u8 buf[512];

    ata_read(0, buf, 512);
    
    return 0;
}

void ata_init() {
    u8 ata_status = ata_identify(ATA_PRIMARY, ATA_MASTER);
    if (ata_status == 1) {
        log_bad("ATA: Couldn't identify disk.\n");
    } else if (ata_status == 2) {
        log_ok("ATA: Disk identified correctly.\n");
        log_bad("ATA: Disk not ready yet.\n");
    } else {
        log_ok("ATA: Disk identified correcly.\n");
        log_ok("ATA: Ready to read.\n");
    }
}
