#pragma once

#include <types.h>

#include <video/libs/tga.h>

typedef struct {
    u32 start_x;
    u32 start_y;

    u32 ls_width;
    u32 ls_height;
    u32 ms_width;
    u32 ms_height;
    u32 rs_width;
    u32 rs_height;
} theme_button_info;

typedef struct {
    u32 start_x;
    u32 start_y;
    
    u32 bar_ls_width;
    u32 bar_ls_height;
    u32 bar_ms_width;
    u32 bar_ms_height;
    u32 bar_rs_width;
    u32 bar_rs_height;

    u32 ls_width;
    u32 ls_height;
    u32 rs_width;
    u32 rs_height;

    u32 bottom_ls_width;
    u32 bottom_ls_height;
    u32 bottom_ms_width;
    u32 bottom_ms_height;
    u32 bottom_rs_width;
    u32 bottom_rs_height;
} theme_window_info;

typedef struct {
    tga_info* image;
    theme_button_info button_info;
    theme_window_info window_info;
} theme_info;

extern theme_info* theme_global;

void theme_init(char* theme);

void theme_crop_rect_into(u32 x, u32 y, u32 w, u32 h, u32 to_w, u32* from, u32* to);