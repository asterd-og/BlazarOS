#include <arch/smp/smp.h>

u32 bsp_lapic_id;
u64 smp_cpu_started = 0;
u64 smp_cpu_count;
cpu_info* smp_cpu_list[256];

struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

void smp_init_cpu(struct limine_smp_info* smp_info) {
    gdt_init();
    idt_reinit();
    vmm_switch_pm(vmm_kernel_pm);

    lapic_init();
    
    while (smp_cpu_started < smp_info->processor_id - 1) {
        __asm__ ("nop");
    }

    cpu_info* cpu = (cpu_info*)kmalloc(sizeof(cpu_info));
    cpu->lock = false;
    cpu->lapic_id = lapic_get_id();

    cpu->proc_size = 0;
    cpu->proc_idx = 0;
    cpu->current_proc = NULL;

    smp_cpu_list[smp_info->processor_id] = cpu;

    sched_init();

    log_info("CPU %lx is up.\n", smp_info->processor_id);
    log_info("CPU %lx lapic id: %x\n", smp_info->processor_id, lapic_get_id());
    smp_cpu_started++;

    while (true) {
        __asm__ ("nop");
    }
}

cpu_info* this_cpu() {
    return smp_cpu_list[lapic_get_id()];
}

void smp_init() {
    struct limine_smp_response* smp_response = smp_request.response;
    
    u64 count = smp_response->cpu_count;
    bsp_lapic_id = smp_response->bsp_lapic_id;
    smp_cpu_count = count;

    cpu_info* cpu0 = (cpu_info*)kmalloc(sizeof(cpu_info));
    cpu0->lock = false;
    cpu0->lapic_id = 0;
    cpu0->proc_size = 0;
    cpu0->proc_idx = 0;
    cpu0->current_proc = NULL;
    smp_cpu_list[0] = cpu0;

    log_info("%d CPUs detected.\n", count);

    for (u64 i = 1; i < count; i++) {
        smp_response->cpus[i]->goto_address = smp_init_cpu;
    }
}

cpu_info* get_cpu(u64 id) {
    if (id > smp_cpu_count) return NULL;
    return smp_cpu_list[id];
}