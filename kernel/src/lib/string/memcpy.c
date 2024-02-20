#include <lib/string/string.h>
#include <types.h>

void *memcpy(void *d, const void *s, size_t n) {
    __asm__ volatile ("rep movsb"
                  : "=D" (d),
                    "=S" (s),
                    "=c" (n)
                  : "0" (d),
                    "1" (s),
                    "2" (n)
                  : "memory");
    return d;
}