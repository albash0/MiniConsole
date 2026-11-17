#pragma once
#include <stdint.h>
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 200
#define DISPLAY_PORTRAIT 0
#define STRIP_ROWS 16
static inline void display_set_viewport(int x,int y,int w,int h) { (void)x;(void)y;(void)w;(void)h; }
