#include <sched/sched.h>
#include <arch/smp/smp.h>

bool sched_initialised = false;

void sched_wrapper(u64 func) {
    ((u64(*)())func)();
    lock();
    this_cpu()->current_proc->state = PROC_DEAD;
    unlock();
    for (;;) {
        __asm__ ("hlt");
    }
}

process* sched_new_proc(void* func) {
    lock();
    process* proc = (process*)kmalloc(sizeof(process));

    proc->PID = this_cpu()->proc_pid;
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

    this_cpu()->proc_list[this_cpu()->proc_pid] = proc;
    this_cpu()->proc_pid++;
    unlock();

    return proc;
}

void sched_kill() {
    lock();
    this_cpu()->current_proc->state = PROC_DEAD;
    unlock();
}

void sched_remove_proc(u64 pid) {
    process* proc = this_cpu()->proc_list[pid];
    
    vmm_destroy_pm(proc->pm);
    kfree((void*)(proc->regs.rsp - PAGE_SIZE));
    kfree(proc);
    
    this_cpu()->proc_list[pid] = NULL;

    if (pid != this_cpu()->proc_pid) {
        for (u64 i = pid; i < this_cpu()->proc_pid; i++) {
            this_cpu()->proc_list[i] = this_cpu()->proc_list[i + 1];
        }
    }
    this_cpu()->proc_pid--;
    log_ok("Killed process %ld.\n", pid);
}

void sched_switch(registers* regs) {
    lock();
    if (this_cpu()->current_proc != NULL) {
        this_cpu()->current_proc->regs = *regs;
    }
    this_cpu()->current_proc = this_cpu()->proc_list[this_cpu()->proc_idx];
    *regs = this_cpu()->current_proc->regs;
    vmm_switch_pm(this_cpu()->current_proc->pm);

    this_cpu()->proc_idx++;
    if (this_cpu()->proc_idx == this_cpu()->proc_pid) {
        this_cpu()->proc_idx = 0;
    }
    if (this_cpu()->proc_list[this_cpu()->proc_idx]->state == PROC_DEAD) {
        sched_remove_proc(this_cpu()->proc_idx);
        this_cpu()->proc_idx = 0;
    }
    unlock();
}

void lock() {
    if (!sched_initialised) return;
    if (this_cpu()->lock) return;
    while (__atomic_test_and_set(&(this_cpu()->lock), __ATOMIC_ACQUIRE));
}

void unlock() {
    if (!sched_initialised) return;
    if (!this_cpu()->lock) return;
    __atomic_clear(&(this_cpu()->lock), __ATOMIC_RELEASE);
}

void sched_test() {
    while (true) {
        serial_printf("%x", lapic_get_id());
    }
}

void sched_init() {
    sched_initialised = true;
    sched_new_proc(sched_test);
    sched_new_proc(sched_test);
    irq_register(SCHED_INT_VEC - 32, sched_switch);
}