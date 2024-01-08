#pragma once

#include <types.h>

#include <arch/idt/idt.h>

#include <mm/heap/heap.h>
#include <mm/vmm/vmm.h>
#include <mm/pmm/pmm.h>

#include <lib/log.h>
#include <lib/string/string.h>

#define SCHED_INT_VEC 0x32

enum {
    PROC_DEAD,
    PROC_RUNNING
};

typedef struct {
    u64 PID;
    registers regs;
    page_map* pm;
    u64 state;
} process;

void sched_init();

process* sched_new_proc(void* func, u64 cpu_id);
void sched_kill();

void sched_switch(registers* regs);

void lock();
void unlock();