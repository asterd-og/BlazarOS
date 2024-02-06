#include <lib/panic.h>
#include <main.h>
void panic(const char* fmt, ...) {
    ft_ctx->cursor_enabled = 0;
    printf("\x1b[1;34;41m\x1b[2J\x1b[1;34;37mPANIC!\n");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
