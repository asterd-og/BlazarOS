#pragma once

#include <types.h>

#include <desktop/wm.h>

typedef struct {
    char* text;
    int text_len;
    bool pressed;
} button_info;

void btn_init();

element_info* btn_create(u32 x, u32 y, char* text, window_info* win);

void btn_draw(element_info* btn);
void btn_update(element_info* btn);