#include "../string.h"

int strcmp(const char* str, const char* str2) {
    if (strlen(str) != strlen(str2))
        return 1;
    
    for (int i = 0; i < strlen(str); i++)
        if (str[i] != str2[i])
            return 1;
    
    return 0;
}