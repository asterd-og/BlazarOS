#include <arch/idt/idt.h>
#include <arch/smp/smp.h>

__attribute__((aligned(0x10)))
static idt_entry idt_entries[256];
static idtr      idt_data;
extern void*     idt_int_table[];

void* irq_handlers[16] = {};

static const char* isr_errors[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void idt_set_entry(u8 vec, void* isr) {
    idt_entries[vec].low  = (u64)isr & 0xFFFF;
    idt_entries[vec].cs   = 0x28;
    idt_entries[vec].ist  = 0;
    idt_entries[vec].attr = (u8)0x8E;
    idt_entries[vec].mid  = ((u64)isr >> 16) & 0xFFFF;
    idt_entries[vec].high = ((u64)isr >> 32) & 0xFFFFFFFF;
    idt_entries[vec].resv = 0;
}

void irq_register(u8 vec, void* handler) {
    ioapic_redirect_irq(bsp_lapic_id, vec + 0x20, true);
    irq_handlers[vec] = handler;
}

void isr_handler(registers* regs) {
    if (regs->int_no > 32) {
        serial_printf("> %lx\n", regs->int_no);
        return;
    }

    __asm__ volatile("cli");

    panic("System Fault!\nMessage: %s\n", isr_errors[regs->int_no]);

    for (;;) __asm__ volatile("hlt");
}

void irq_handler(registers* regs) {
    void(*handler)(registers*);
    handler = irq_handlers[regs->int_no - 32];

    if (handler != NULL)
        handler(regs);
    
    lapic_eoi();
}

void idt_reinit() {
    __asm__ ("lidt %0" : : "m"(idt_data) : "memory");
    __asm__ ("sti");
}

void idt_init() {

    for (u16 vec = 0; vec < 256; vec++)
        idt_set_entry(vec, idt_int_table[vec]);
    
    idt_data = (idtr){
        .size   = (u16)sizeof(idt_entry) * 256 - 1,
        .offset = (u64)idt_entries
    };

    __asm__ ("lidt %0" : : "m"(idt_data) : "memory");
    
    __asm__ ("sti");
}