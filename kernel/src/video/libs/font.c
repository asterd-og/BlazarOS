#include <video/libs/font.h>
#include <mm/heap/heap.h>
#include <video/libs/fonts_data.h>

font_info** fonts_arr = NULL;
u8 font_arr_idx = 0;

void font_init() {
    fonts_arr = (font_info**)kmalloc(sizeof(font_info) * 16);
    font_info* font_slim_8x16 = (font_info*)kmalloc(sizeof(font_info));
    font_slim_8x16->width = 8;
    font_slim_8x16->height = 16;
    font_slim_8x16->spacing_x = 0;
    font_slim_8x16->spacing_y = 0;
    font_slim_8x16->data = kmalloc(sizeof(FONTDATA_SLIM_8x16));
    memcpy(font_slim_8x16->data, FONTDATA_SLIM_8x16, sizeof(FONTDATA_SLIM_8x16));
    fonts_arr[0] = font_slim_8x16;
}

font_info* font_get(u8 idx) {
    return fonts_arr[idx];
}