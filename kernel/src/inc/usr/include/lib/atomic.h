#pragma once

#include <types.h>

typedef struct {
    bool locked;
} locker_info;

void lock(locker_info* lock);
void unlock(locker_info* lock);