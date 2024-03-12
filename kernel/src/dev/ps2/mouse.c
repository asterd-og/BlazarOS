#include <dev/ps2/mouse.h>
#include <lib/atomic.h>

u8 mouse_state = 0;
u8 mouse_flags = 0;
i32 mouse_bytes[2] = {0, 0};

u32 mouse_x = 20;
u32 mouse_y = 20;

bool mouse_left_pressed = false;
bool mouse_right_pressed = false;

bool mouse_moved = false;

static inline void mouse_wait_write() {
    int timeout = 100000;
    while (timeout--) {
        if ((inb(0x64) & 2) == 0) {
            return;
        }
    }
}

static inline void mouse_wait_read() {
    int timeout = 100000;
    while (timeout--) {
        if ((inb(0x64) & 1) == 1) {
            return;
        }
    }
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

bool discard_packet = false;
locker_info mouse_lock;

i32 mouse_wrap_x = 20;
i32 mouse_wrap_y = 20;

void mouse_handler(registers* regs) {
    (void)regs;
    lock(&mouse_lock);
    switch (mouse_state) {
        // Packet state
        case 0:
            mouse_flags = mouse_read();
            mouse_state++;
            if (mouse_flags & (1 << 6) || mouse_flags & (1 << 7))
                discard_packet = true;
            if (!(mouse_flags & (1 << 3)))
                discard_packet = true;
            break;
        case 1:
            mouse_bytes[0] = mouse_read();
            mouse_state++;
            break;
        case 2:
            mouse_bytes[1] = mouse_read();
            mouse_state = 0;

            if (!discard_packet) {
                if (mouse_flags & (1 << 4))
                    mouse_bytes[0] = (i8)(u8)mouse_bytes[0];
                if (mouse_flags & (1 << 5))
                    mouse_bytes[1] = (i8)(u8)mouse_bytes[1];
                
                mouse_x = mouse_x + (mouse_bytes[0]);
                mouse_y = mouse_y - (mouse_bytes[1]);

                if ((i32)mouse_x <= 1) mouse_x = 1;
                if ((i32)mouse_y <= 1) mouse_y = 1;
                if ((i32)mouse_x >= vbe->width - 1) mouse_x = vbe->width - 1;
                if ((i32)mouse_y >= vbe->height - 1) mouse_y = vbe->height - 1;

                mouse_moved = true;
                wm_mouse_task();
                mouse_left_pressed = (bool)(mouse_flags & 0b00000001);
                mouse_right_pressed = (bool)((mouse_flags & 0b00000010) >> 1);
            } else {
                discard_packet = false;
            }
            break;
    }
    unlock(&mouse_lock);
}

void mouse_init() {
    mouse_write(0xf6);
    mouse_read();

    mouse_write(0xf4);
    mouse_read();

    irq_register(12, mouse_handler);
}