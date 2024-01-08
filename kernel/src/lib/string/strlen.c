#include <lib/string/string.h>

int strlen(const char* str) {
    int i = 0;
    while (*str != '\0') {
        i++;
        str++;
    }
    return i;
}