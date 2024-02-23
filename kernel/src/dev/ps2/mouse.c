#include <dev/ps2/mouse.h>

u8 mouse_state = 0;
u8 mouse_bytes[3] = {0, 0, 0};

u32 mouse_x = 0;
u32 mouse_y = 0;

i32 mouse_wrap_x = 0;
i32 mouse_wrap_y = 0;

bool mouse_left_pressed = false;
bool mouse_right_pressed = false;

bool mouse_moved = false;

void mouse_wait_write() {
    while ((inb(0x64) & 2) != 0) {;}
}

void mouse_wait_read() {
    while ((inb(0x64) & 1) != 1) {;}
}

void mouse_write(u8 value) {
    mouse_wait_write();
    outb(0x64, 0xd4);
    mouse_wait_write();
    outb(0x60, value);
}

u8 mouse_read() {
    mouse_wait_read();
    return inb(0x60);
}

void mouse_update(i8 accel_x, i8 accel_y) {
    if (mouse_wrap_x + accel_x <= 0) { mouse_wrap_x = 0; return; }
    if (mouse_wrap_y - accel_y <= 0) { mouse_wrap_y = 0; return; }
    if (mouse_wrap_x + accel_x > (i32)vbe->width) { mouse_wrap_x = vbe->width; return; }
    if (mouse_wrap_y - accel_y > (i32)vbe->height) { mouse_wrap_y = vbe->height; return ;}

    mouse_wrap_x += accel_x;
    mouse_wrap_y -= accel_y;

    mouse_x = (u32)mouse_wrap_x;
    mouse_y = (u32)mouse_wrap_y;
}

void mouse_handler(registers* regs) {
    (void)regs;
    u8 byte = inb(0x64);
    if ((!(byte & 1)) == 1) { mouse_state = 0; return; }
    if ((!(byte & 2)) == 0) { mouse_state = 0; return; }
    if (!(byte & 0x20)) { mouse_state = 0; return; }
    switch (mouse_state) {
        // Packet state
        case 0:
            mouse_wait_read();
            mouse_bytes[0] = mouse_read();
            mouse_state++;
            break;
        case 1:
            mouse_wait_read();
            mouse_bytes[1] = mouse_read();
            mouse_state++;
            break;
        case 2:
            mouse_wait_read();
            mouse_bytes[2] = mouse_read();
            
            if (mouse_bytes[0] & 0x80 || mouse_bytes[0] & 0x40) return;

            mouse_update(mouse_bytes[1], mouse_bytes[2]);

            mouse_left_pressed = (bool)(mouse_bytes[0] & 0b00000001);
            mouse_right_pressed = (bool)((mouse_bytes[0] & 0b00000010) >> 1);

            mouse_state = 0;
            mouse_moved = true;
            break;
    }
}

void mouse_init() {
    u8 data;
    mouse_wait_write();
    outb(0x64, 0xa8);
    mouse_wait_write();
    outb(0x64, 0x20);

    mouse_read();
    data = (inb(0x60) | 2);

    mouse_wait_write();
    outb(0x64, 0x60);
    mouse_wait_write();
    outb(0x60, data);

    mouse_write(0xf6);
    mouse_read();
    mouse_write(0xf4);
    mouse_read();

    irq_register(12, mouse_handler);
}