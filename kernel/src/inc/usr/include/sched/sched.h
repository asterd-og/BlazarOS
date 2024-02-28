#pragma once

#include <types.h>

#include <arch/idt/idt.h>

#include <mm/heap/heap.h>
#include <mm/vmm/vmm.h>
#include <mm/pmm/pmm.h>

#include <lib/log.h>
#include <lib/string/string.h>

#include <sys/elf.h>

#define SCHED_INT_VEC 0x32

extern u64 sched_pid;

enum {
    PROC_DEAD,
    PROC_RUNNING,
    PROC_IDLE
};

enum {
    PROC_PR_LOW = 1,
    PROC_PR_NORMAL = 2,
    PROC_PR_MEDIUM = 4,
    PROC_PR_HIGH = 6
};

typedef struct {
    u64 PID;
    registers regs;
    page_map* pm;
    u8 priority;
    u64 state;
    u64 cpu_id;
    u64 stack_addr;
} process;

void sched_init();

process* sched_new_proc(void* func, u64 cpu_id, u8 priority);
process* sched_new_elf(void* elf, u64 cpu_id, u8 priority);
process* sched_get_proc(u64 pid);
void sched_kill();

void sched_switch(registers* regs);