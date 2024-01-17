#include "../string.h"

// Function provided by KM198912
// (https://github.com/KM198912)

char* strtok(char* str, const char* delim) {
    static char* p = NULL;
    if (str != NULL) { p = str; }
    else if (p == NULL) { return NULL; }

    char* start = p;
    while (*p != '\0')  {
        const char* d = delim;
        while (*d != '\0') {
            if (*p == *d) {
                *p = '\0';
                p++;
                if (start == p) {
                    start = p;
                    continue;
                }
                return start;
            }
            d++;
        }
        p++;
    }
    if (start == p) { return NULL; }
    return start;
}