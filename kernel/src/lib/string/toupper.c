#include <lib/string/string.h>

int toupper(int c) {
    if (c >= 'A' && c <= 'Z') return c;
    return (c + ('A' - 'a'));
}