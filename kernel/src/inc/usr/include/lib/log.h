#pragma once

#include <types.h>
#include <lib/stdio/printf.h>

void log_ok(const char* fmt, ...);
void log_bad(const char* fmt, ...);
void log_info(const char* fmt, ...);