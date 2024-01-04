#include <sched/pit.h>
#include <serial/serial.h>

void pit_handler(registers* regs) {
    serial_printf("LOLL");
    sched_switch(regs);
}

void pit_init() {
    outb(0x43, 0x36);
    u64 div = 1193180 / PIT_BASE_FREQ;
    outb(0x40, (u8)div);
    outb(0x40, (u8)(div >> 8));
    irq_register(0, pit_handler);
}