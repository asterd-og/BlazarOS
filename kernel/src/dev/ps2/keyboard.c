#include <dev/ps2/keyboard.h>

bool keyboard_pressed = false;
char keyboard_char = '\0';
bool keyboard_caps = false;
bool keyboard_shift = false;

void keyboard_handler(registers* regs) {
    (void)regs;

    u8 key = inb(0x60);

    if (!(key & 0x80)) {
        switch (key) {
            case 0x2a:
                // Shift
                keyboard_shift = true;
                break;
            case 0x3a:
                // Caps
                keyboard_caps = !keyboard_caps;
                break;
            default:
                // Letter
                keyboard_pressed = true;
                if (keyboard_shift) keyboard_char = kb_map_keys_shift[key];
                else if (keyboard_caps) keyboard_char = kb_map_keys_caps[key];
                else keyboard_char = kb_map_keys[key];
                break;
        }
    } else {
        switch (key) {
            case 0xaa:
                // Shift
                keyboard_shift = false;
                break;
        }
    }
}

char keyboard_get() {
    if (keyboard_pressed) {
        keyboard_pressed = false;
        return keyboard_char;
    }
    return '\0';
}

void keyboard_init() {
    irq_register(1, keyboard_handler);
}