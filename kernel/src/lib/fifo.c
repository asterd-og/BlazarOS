#include <lib/fifo.h>
#include <mm/heap/heap.h>

fifo* fifo_create(u64 size) {
    fifo* ret = (fifo*)kmalloc(sizeof(fifo));
    ret->data = (u8*)kmalloc(size);
    memset(ret->data, 0, size);
    ret->size = size;
    ret->idx = 0;
    ret->count = 0;
    ret->lock.locked = false;
    return ret;
}

void fifo_push(fifo* q, u8 data) {
    lock(&q->lock);
    if (q->count == q->size) return;

    q->data[q->count++] = data;
    unlock(&q->lock);
}

u8 fifo_pop(fifo* q) {
    lock(&q->lock);
    u8 data = q->data[q->idx];
    unlock(&q->lock);
    if (data == 0) return 0;
    q->idx++;
    return data;
}

void fifo_clear(fifo* q) {
    memset(q->data, 0, q->size);
    q->count = 0;
    q->idx = 0;
}