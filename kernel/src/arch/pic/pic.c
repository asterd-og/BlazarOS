#include <arch/pic/pic.h>
#include <arch/io.h>

void pic_eoi(u8 no) {
    if (no > 7)
        outb(PIC2_CMD, PIC_EOI);
    
    outb(PIC1_CMD, PIC_EOI);
}

void pic_disable() {
    outb(PIC1_DAT, 0xff);
    outb(PIC2_DAT, 0xff);
}

void pic_remap() {
    // This will start the init sequence in cascade mode
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DAT, 0x20); // Master PIC offset
    outb(PIC2_DAT, 0x28); // Slave  PIC offset
    outb(PIC1_DAT, 0x04); // Tell master PIC there's slave PIC at IRQ2
    outb(PIC2_DAT, 0x02); // Tell slave PIC it's cascade identity
    outb(PIC1_DAT, ICW4_8086); // Use 8086 PIC
    outb(PIC2_DAT, ICW4_8086);
    outb(PIC1_DAT, 0); // Set mask
    outb(PIC2_DAT, 0);
}