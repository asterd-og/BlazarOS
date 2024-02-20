#include <video/vbe.h>
#include <video/framebuffer.h>
#include <mm/heap/heap.h>
#include <dev/initrd/blazfs.h>
#include <lib/atomic.h>

struct limine_framebuffer_response* limine_fb = NULL;
u32* lfb_buffer = NULL;
u32* vbe_buffer = NULL;
u32 vbe_size = 0;
framebuffer_info* vbe = NULL;
locker_info vbe_lock = {.locked = false};

void vbe_init(struct limine_framebuffer* fb) {
    limine_fb = fb;
    lfb_buffer = fb->address;
    vbe_size = fb->width * fb->height * 4;
    vbe_buffer = (u32*)kmalloc(vbe_size);
    vbe = fb_create(vbe_buffer, fb->width, fb->height, fb->pitch);

    fb_clear(vbe, 0xFFFFFFFF);
}

void vbe_swap() {
    memcpy(lfb_buffer, vbe_buffer, vbe_size);
}