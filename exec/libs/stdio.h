#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

int vsprintf(char* buf, const char* fmt, va_list args);
int printf(const char* fmt, ...);