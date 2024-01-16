#pragma once

#include <types.h>

#include <arch/idt/idt.h>

#include <sched/sched.h>

void syscall_handler(registers* regs);