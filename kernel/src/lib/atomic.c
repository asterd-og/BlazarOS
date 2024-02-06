#include <lib/atomic.h>

void lock(locker_info* lock) {
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE)) {
        __asm__ volatile("nop");
    }
}

void unlock(locker_info* lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}