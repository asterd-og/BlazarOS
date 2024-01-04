#include <arch/smp/smp.h>
#include <serial/serial.h>

u32 bsp_lapic_id;

struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

void smp_init_cpu(struct limine_smp_info* smp_info) {
    gdt_init();
    idt_reinit();
    vmm_switch_pm(vmm_kernel_pm);

    char* stack = (char*)pmm_alloc(8); // ≃ 32 kib
    __asm__ ("mov %0, %%rsp" : : "r"(stack) : "memory");

    lapic_init_cpu(smp_info->lapic_id);
    lapic_start_cpu(smp_info->lapic_id, 0x800);
    
    log_info("CPU %lx is up.\n", smp_info->processor_id);

    while (true) __asm__ ("hlt");
}

void smp_init() {
    struct limine_smp_response* smp_response = smp_request.response;
    
    u64 count = smp_response->cpu_count;
    bsp_lapic_id = smp_response->bsp_lapic_id;

    for (u64 i = 1; i < count; i++) {
        smp_response->cpus[i]->goto_address = smp_init_cpu;
    }
}