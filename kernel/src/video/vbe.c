#include <video/vbe.h>
#include <video/framebuffer.h>
#include <mm/heap/heap.h>
#include <dev/initrd/blazfs.h>
#include <lib/atomic.h>

struct limine_framebuffer_response* limine_fb;
u32* lfb_buffer;
u32* vbe_buffer;
u32 vbe_size;
framebuffer_info* vbe;
locker_info vbe_lock;

void vbe_init(struct limine_framebuffer* fb) {
    limine_fb = fb;
    lfb_buffer = fb->address;
    vbe_size = fb->width * fb->height * 4;
    vbe_buffer = (u32*)kmalloc(vbe_size);
    vbe = fb_create(vbe_buffer, fb->width, fb->height, fb->pitch);

    u8* font_buffer = kmalloc(blazfs_ftell("FreeSans.sfn"));
    blazfs_read("FreeSans.sfn", font_buffer);
    fb_set_font(vbe, SSFN_FAMILY_ANY, 24, font_buffer);

    fb_clear(vbe, 0xFFFFFFFF);
}

void vbe_swap() {
    lock(&vbe_lock);
    memcpy(lfb_buffer, vbe_buffer, vbe_size);
    unlock(&vbe_lock);
}