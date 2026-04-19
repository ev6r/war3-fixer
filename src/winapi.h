#ifndef WINAPI_H
#define WINAPI_H

#include <windows.h>

#define MAX_PATH 260
#define MAX_PROCESSES 1024

HWND find_window(const char *class_name, const char *window_name);
HWND get_child_window(HWND hwnd);
BOOL get_window_rect(HWND hwnd, RECT *rect);
BOOL set_window_pos_hw(HWND hwnd, int x, int y, int width, int height);
BOOL is_process_running(const char *process_name);
BOOL start_process(const char *path, const char *params);
void log_init(const char *filename);
void log_close(void);
void log_message(const char *msg);

#endif