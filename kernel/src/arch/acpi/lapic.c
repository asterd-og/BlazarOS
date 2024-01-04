#include <arch/acpi/lapic.h>

void lapic_write(u32 reg, u32 val) {
    mmio_write32(HIGHER_HALF(lapic_addr) + reg, val);
}

u32 lapic_read(u32 reg) {
    return mmio_read32(HIGHER_HALF(lapic_addr) + reg);
}

void lapic_eoi() {
    lapic_write(0x0b0, 0x0);
}

void lapic_ipi(u32 id, u32 dat) {
    lapic_write(LAPIC_ICRHI, id << LAPIC_ICDESTSHIFT);
    lapic_write(LAPIC_ICRLO, dat);

    while ((lapic_read(LAPIC_ICRLO) & LAPIC_ICPEND) != 0)
        ;
}

void lapic_init_cpu(u32 id) {
    lapic_ipi(id, (u32)(LAPIC_ICINI | LAPIC_ICPHYS | LAPIC_ICASSR |
       LAPIC_ICEDGE | LAPIC_ICSHRTHND));
}

void lapic_start_cpu(u32 id, u32 vec) {
    lapic_ipi(id, vec | LAPIC_ICSTR | LAPIC_ICPHYS | LAPIC_ICASSR |
       LAPIC_ICEDGE | LAPIC_ICSHRTHND);
}

u32 lapic_get_id() {
    return lapic_read(0x0020) >> LAPIC_ICDESTSHIFT;
}

void lapic_init() {
    lapic_write(0x00f0, lapic_read(0x00f0) | (1 << 8) | 0xff);
}