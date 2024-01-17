#include <sys/syscall.h>
#include <dev/ps2/keyboard.h>

void syscall_handler(registers* regs) {
    lock();
    switch (regs->rax) {
        case 1:
            // print
            printf("%s", (char*)regs->rbx);
            break;
        case 2:
            regs->rax = keyboard_get();
            break;
    }
    unlock();
}