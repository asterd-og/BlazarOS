#pragma once

#include <types.h>
#include <lib/atomic.h>

typedef struct {
    locker_info lock;
    u64 idx;
    u64 size;
    u64 count;
    u8* data;
} fifo;

fifo* fifo_create(u64 size);
void fifo_push(fifo* q, u8 data);
u8 fifo_pop(fifo* q);
void fifo_clear(fifo* q);