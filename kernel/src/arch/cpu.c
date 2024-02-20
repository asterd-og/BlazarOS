#include <arch/cpu.h>
#include <mm/memory.h>

void sse_init() {
    u64 cr0 = read_cr0();
    cr0 &= ~(1 << 2);
    write_cr0(cr0 | (1 << 1));
    u64 cr4 = read_cr4();
    cr4 |= (3 << 9);
    write_cr4(cr4);
}