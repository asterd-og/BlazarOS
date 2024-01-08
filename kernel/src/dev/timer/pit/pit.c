#include <dev/timer/pit/pit.h>

u64 pit_ticks = 0;

void pit_handler(registers* regs) {
    (void)regs;
    pit_ticks++;
    lapic_send_all_int(0, SCHED_INT_VEC);
}

void pit_reset() {
    irq_register(0, pit_handler);
}

void pit_init() {
    outb(0x43, 0x36);
    u64 div = 1193180 / PIT_BASE_FREQ;
    outb(0x40, (u8)div);
    outb(0x40, (u8)(div >> 8));
    irq_register(0, pit_handler);
}