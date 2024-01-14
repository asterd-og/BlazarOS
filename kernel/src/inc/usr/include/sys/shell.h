#pragma once

#include <types.h>

#include <fs/vfs.h>

#include <lib/string/string.h>
#include <lib/stdio/printf.h>

#include <flanterm/colors.h>

#include <dev/ps2/keyboard.h>

void shell_init();

void shell_update();