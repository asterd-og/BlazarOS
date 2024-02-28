#include <sys/syscall.h>
#include <dev/ps2/keyboard.h>

void syscall_handler(registers* regs) {
    switch (regs->rax) {
    }
}