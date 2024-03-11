#include <dev/ps2/keyboard.h>
#include <lib/atomic.h>
#include <desktop/wm.h>

bool keyboard_pressed = false;
bool keyboard_pressed_old = false;

char keyboard_char = '\0';
bool keyboard_caps = false;
bool keyboard_shift = false;

fifo* keyboard_fifo = NULL;

locker_info keyboard_lock;

void keyboard_handle_key(u8 key) {
    lock(&keyboard_lock);
    switch (key) {
        case 0x2a:
            // Shift
            keyboard_shift = true;
            break;
        case 0xaa:
            keyboard_shift = false;
            break;
        case 0x3a:
            // Caps
            keyboard_caps = !keyboard_caps;
            break;
        default:
            // Letter
            if (!(key & 0x80)) {
                keyboard_pressed = true;
                if (keyboard_shift) keyboard_char = kb_map_keys_shift[key];
                else if (keyboard_caps) keyboard_char = kb_map_keys_caps[key];
                else keyboard_char = kb_map_keys[key];
                fifo_push(keyboard_fifo, keyboard_char);
            }
            break;
    }
    unlock(&keyboard_lock);
}

void keyboard_handler(registers* regs) {
    (void)regs;

    u8 status;
    u8 key;

    lapic_eoi();

    status = inb(0x64);
    if (status & 1) {
        key = inb(0x60);
        keyboard_handle_key(key);
    } else {
        keyboard_char = 0;
        keyboard_pressed = false;
    }
}

char keyboard_get() {
    char c = keyboard_char;
    keyboard_char = 0;
    return c;
}

// This part and mouse's handler is from lyreOS
// I got it from them because I thought a mouse bug was a bug in my code
// But turns out it was in qemu's version (mine was older)
// And now I'm too lazy to change back!

u8 keyboard_read() {
    while ((inb(0x64) & 1) == 0);
    return inb(0x60);
}

void keyboard_write(u16 port, u8 val) {
    while ((inb(0x64) & 2) != 0);
    outb(port, val);
}

u8 keyboard_read_cfg() {
    keyboard_write(0x64, 0x20);
    return keyboard_read();
}

void keyboard_write_cfg(u8 cfg) {
    keyboard_write(0x64, 0x60);
    keyboard_write(0x60, cfg);
}

void keyboard_init() {
    keyboard_write(0x64, 0xad);
    keyboard_write(0x64, 0xa7);

    u8 cfg = keyboard_read_cfg();
    cfg |= (1 << 0) | (1 << 6);
    if ((cfg & (1 << 5)) != 0) {
        cfg |= (1 << 1);
    }
    keyboard_write_cfg(cfg);

    keyboard_write(0x64, 0xae);
    if ((cfg & (1 << 5)) != 0) {
        keyboard_write(0x64, 0xa8);
    }

    keyboard_fifo = fifo_create(512);
    irq_register(1, keyboard_handler);
    inb(0x60);
}