#include <sys/syscall.h>

void syscall_handler(registers* regs) {
    lock();
    switch (regs->rax) {
        case 1:
            // print
            printf("%s", (char*)regs->rbx);
            break;
    }
    unlock();
}