#include <lib/panic.h>
#include <main.h>
void panic(const char* fmt, ...) {
    ft_ctx->cursor_enabled = 0;
    printf("PANIC!\n");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
