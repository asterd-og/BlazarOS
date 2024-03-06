#include <dev/timer/hpet/hpet.h>

hpet_info* hpet;
u64 hpet_comp_update;

void hpet_write(u64 reg, u64 val) {
    mmio_write64(HIGHER_HALF(hpet->addr_info.address + reg), val);
}

u64 hpet_read(u64 reg) {
    return mmio_read64(HIGHER_HALF(hpet->addr_info.address + reg));
}

void hpet_write_bits(u64 reg, u64 bits) {
    hpet_write(reg, hpet_read(reg) | bits);
}

void hpet_clear_bits(u64 reg, u64 bits) {
    hpet_write(reg, hpet_read(reg) & ~bits);
}

u64 hpet_timer_config_reg(u64 n) {
    return (0x100 + 0x20 * n);
}

u64 hpet_timer_comp_reg(u64 n) {
    return (0x108 + 0x20 * n);
}

u64 hpet_timer_int_reg(u64 n) {
    return (0x110 + 0x20 * n);
}

u64 ticks = 0;

void hpet_handler(registers* regs) {
    (void)regs;
    ticks++;
    hpet_write_bits(0x20, 1 << 0);
    hpet_write(hpet_timer_comp_reg(0), hpet_read(HPET_COUNTER) + hpet_comp_update);
    if ((ticks % 50) == 0) {
        lapic_send_others_int(0, SCHED_INT_VEC);
    }
}

void hpet_init() {
    hpet = (hpet_info*)acpi_find_table("HPET");

    hpet_clear_bits(HPET_CONFIG, 1 << 1);

    u64 hpet_period = hpet_read(0) >> 32;
    u64 hpet_freq = 1000000000000000 / hpet_period;

    u64 cur_freq = hpet_freq;
    if (hpet_freq >= 10000) {
        cur_freq = 10000;
    } else {
        cur_freq = hpet_freq;
    }

    hpet_comp_update = hpet_freq / cur_freq;

    irq_register(0, hpet_handler);

    hpet_write(HPET_COUNTER, 0);
    
    hpet_clear_bits(hpet_timer_config_reg(0), 1 << 3); // periodic
    hpet_write_bits(hpet_timer_config_reg(0), 1 << 2); // enable
    hpet_write(hpet_timer_comp_reg(0), hpet_comp_update); // enable

    hpet_write_bits(HPET_CONFIG, 1 << 0 | 1 << 1);
}