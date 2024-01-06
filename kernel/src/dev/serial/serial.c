#include <dev/serial/serial.h>

int serial_bus_status() {
    return inb(COM1 + 5) & 0x20;
}

void serial_write_char(char c, void* extra) {
    (void)extra;
    while (serial_bus_status() == 0);
    outb(COM1, c);
}

void serial_printf(const char* format, ...) {
    lock();
    
    va_list args;
    va_start(args, format);
    vfctprintf(serial_write_char, NULL, format, args);
    va_end(args);
    
    unlock();
}

void serial_init() {
    outb(COM1 + 1, 0);
    outb(COM1 + 3, 0x80);
    outb(COM1, 0x03);
    outb(COM1 + 1, 0);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x1E);
    outb(COM1, 0xAE);
    outb(COM1 + 4, 0x0F);
}