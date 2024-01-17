#include "../stdio.h"

// Function provided by KM198912
// (https://github.com/KM198912)

int vsprintf(char* buf, const char* fmt, va_list args) {
    char* buf_ptr = buf;
    const char* fmt_ptr = fmt;
    while (*fmt_ptr != '\0') {
        if (*fmt_ptr == '%') {
            fmt_ptr++;
            switch (*fmt_ptr) {
                case 's': {
                    char* str = va_arg(args, char*);
                    while (*str != '\0') {
                        *buf_ptr = *str;
                        buf_ptr++;
                        str++;
                    }
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    char num_buf[32];
                    int num_buf_ptr = 0;
                    if (num < 0) {
                        *buf_ptr = '-';
                        buf_ptr++;
                        num = -num;
                    }
                    while (num > 0) {
                        num_buf[num_buf_ptr] = (num % 10) + '0';
                        num_buf_ptr++;
                        num /= 10;
                    }
                    num_buf_ptr--;
                    while (num_buf_ptr >= 0) {
                        *buf_ptr = num_buf[num_buf_ptr];
                        buf_ptr++;
                        num_buf_ptr--;
                    }
                    break;
                }
                case 'x': {
                    int num = va_arg(args, int);
                    char num_buf[32];
                    int num_buf_ptr = 0;
                    if (num < 0) {
                        *buf_ptr = '-';
                        buf_ptr++;
                        num = -num;
                    }
                    while (num > 0) {
                        int rem = num % 16;
                        if (rem < 10) {
                            num_buf[num_buf_ptr] = rem + '0';
                        } else {
                            num_buf[num_buf_ptr] = rem - 10 + 'a';
                        }
                        num_buf_ptr++;
                        num /= 16;
                    }
                    num_buf_ptr--;
                    while (num_buf_ptr >= 0) {
                        *buf_ptr = num_buf[num_buf_ptr];
                        buf_ptr++;
                        num_buf_ptr--;
                    }
                    break;
                }
                case 'c': {
                    char c = va_arg(args, int);
                    *buf_ptr = c;
                    buf_ptr++;
                    break;
                }
                case '%': {
                    *buf_ptr = '%';
                    buf_ptr++;
                    break;
                }
                default: {
                    *buf_ptr = '%';
                    buf_ptr++;
                    *buf_ptr = *fmt_ptr;
                    buf_ptr++;
                    break;
                }
            }
        } else {
            *buf_ptr = *fmt_ptr;
            buf_ptr++;
        }
        fmt_ptr++;
    }
    *buf_ptr = '\0';
    return buf_ptr - buf;
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[4096];
    int ret = vsprintf(buf, fmt, args);
    int vec = 1;
    void* addr = buf;
    asm volatile("int $0x80\n\t"
        : "+a"(vec), "+b"(addr)
        :
        : "memory");
    va_end(args);
    return ret;
}