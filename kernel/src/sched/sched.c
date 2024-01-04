#include <sched/sched.h>

process* sched_proc_pool[256] = {};
process* sched_current_proc;
u64 sched_pid = 0;
u64 sched_idx = 0;

bool sched_locked = false;

void sched_wrapper(u64 func) {
    ((u64(*)())func)();
    lock();
    sched_current_proc->state = PROC_DEAD;
    unlock();
    for (;;) {
        __asm__ ("hlt");
    }
}

process* sched_new_proc(void* func) {
    lock();
    process* proc = (process*)kmalloc(sizeof(process));

    proc->PID = sched_pid;
    proc->pm = vmm_new_pm();
    proc->state = PROC_RUNNING;

    proc->regs.rip = (u64)sched_wrapper;
    proc->regs.rdi = (u64)func;
    proc->regs.cs  = 0x28;
    proc->regs.ss  = 0x30;
    proc->regs.rflags = 0x202;

    char* stack = (char*)HIGHER_HALF(pmm_alloc(1));
    // Stack's size is PAGE_SIZE
    proc->regs.rsp = (u64)(stack + PAGE_SIZE);

    sched_proc_pool[sched_pid] = proc;
    sched_pid++;
    unlock();

    return proc;
}

void sched_kill() {
    lock();
    sched_current_proc->state = PROC_DEAD;
    unlock();
}

void sched_remove_proc(u64 pid) {
    process* proc = sched_proc_pool[pid];
    
    vmm_destroy_pm(proc->pm);
    kfree((void*)(proc->regs.rsp - PAGE_SIZE));
    kfree(proc);
    
    sched_proc_pool[pid] = NULL;

    if (pid != sched_pid) {
        for (u64 i = pid; i < sched_pid; i++) {
            sched_proc_pool[i] = sched_proc_pool[i + 1];
        }
    }
    sched_pid--;
    log_ok("Killed process %ld.\n", pid);
}

void sched_switch(registers* regs) {
    lock();
    if (sched_current_proc) {
        sched_current_proc->regs = *regs;
    }
    sched_current_proc = sched_proc_pool[sched_idx];
    *regs = sched_current_proc->regs;
    vmm_switch_pm(sched_current_proc->pm);

    sched_idx++;
    if (sched_idx == sched_pid) {
        sched_idx = 0;
    }
    if (sched_proc_pool[sched_idx]->state == PROC_DEAD) {
        sched_remove_proc(sched_idx);
        sched_idx = 0;
    }
    unlock();
}

void lock() {
    if (sched_locked) return;
    while (__atomic_test_and_set(&sched_locked, __ATOMIC_ACQUIRE));
}

void unlock() {
    if (!sched_locked) return;
    __atomic_clear(&sched_locked, __ATOMIC_RELEASE);
}