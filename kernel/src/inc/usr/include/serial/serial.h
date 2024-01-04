#pragma once

#include <types.h>

#include <arch/io.h>

#include <lib/stdio/printf.h>
#include <lib/string/string.h>

#include <sched/sched.h>

#define COM1 0x3F8

void serial_init();

void serial_printf(const char* format, ...);