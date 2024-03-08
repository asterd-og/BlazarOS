#pragma once

#include <types.h>
#include <desktop/wm.h>

window_info* window_create(u32 x, u32 y, u32 width, u32 height, const char* name);

void window_draw_decorations(window_info*);
void window_update(window_info* win);

void window_add_element(window_info* win, element_info* element);