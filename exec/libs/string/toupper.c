#include "../string.h"

int toupper(int c) {
    if (c >= 'A' && c <= 'Z') return c;
    if (c >= 'a' && c <= 'z') return (c + ('A' - 'a'));
    return c;
}