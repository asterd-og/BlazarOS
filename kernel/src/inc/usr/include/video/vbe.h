#pragma once

#include <types.h>
#include <limine.h>

void vbe_init(struct limine_framebuffer* fb);
void vbe_swap();