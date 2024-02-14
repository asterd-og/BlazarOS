#pragma once

#include <types.h>

#include <desktop/wm.h>

typedef struct {
    char* text;
    bool pressed;
} button_info;

element_info* btn_create(u32 x, u32 y, u32 w, u32 h, char* text, window_info* win);

void btn_draw(element_info* btn);
void btn_update(element_info* btn);