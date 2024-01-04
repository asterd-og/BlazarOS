#include <lib/log.h>

void log_ok(const char* fmt, ...) {
    printf("\x1b[1;34;32m[OK]\x1b[1;34;0m ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void log_bad(const char* fmt, ...) {
    printf("\x1b[1;34;31m[BAD]\x1b[1;34;0m ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void log_info(const char* fmt, ...) {
    printf("\x1b[1;34;36m[INFO]\x1b[1;34;0m ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}