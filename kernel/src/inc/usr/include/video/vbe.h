#pragma once

#include <types.h>
#include <limine.h>

extern u32* lfb_buffer;

void vbe_init(struct limine_framebuffer* fb);
void vbe_swap();