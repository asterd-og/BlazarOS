#pragma once

#include <types.h>

#include <arch/idt/idt.h>
#include <arch/io.h>

#include <dev/ps2/keyboard_map.h>

#include <sched/sched.h>

#include <lib/fifo.h>

extern fifo* keyboard_fifo;

void keyboard_init();
char keyboard_get();

extern bool keyboard_pressed;