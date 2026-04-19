#ifndef FIXER_H
#define FIXER_H

#include <windows.h>

HWND find_war3_window(void);
int set_borderless_fullscreen(HWND hwnd);
int set_window_normal(HWND hwnd);
int fix_window(HWND hwnd, int threshold_ms);
int lock_cursor(HWND hwnd);
int unlock_cursor(void);
int is_window_foreground(HWND hwnd);

#endif