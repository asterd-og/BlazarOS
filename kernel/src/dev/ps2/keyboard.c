#include <dev/ps2/keyboard.h>
#include <lib/atomic.h>

bool keyboard_pressed = false;
bool keyboard_pressed_old = false;

char keyboard_char = '\0';
bool keyboard_caps = false;
bool keyboard_shift = false;

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

void keyboard_init() {
    irq_register(1, keyboard_handler);
}