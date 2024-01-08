#pragma once

#include <types.h>

#include <arch/idt/idt.h>
#include <arch/io.h>

#include <sched/sched.h>

#define PIT_BASE_FREQ 100

void pit_init();

void pit_reset();