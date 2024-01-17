#include "../../blazsys.h"

int keyboard_get() {
    return syscall(2, 0, 0);
}