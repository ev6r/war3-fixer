#include "winapi.h"
#include <stdio.h>
#include <tlhelp32.h>
#include <time.h>

extern int g_show_log;
extern char g_log_file[MAX_PATH];

static FILE *log_fp = NULL;

void log_init(const char *filename) {
    if (!filename || !*filename) return;
    log_fp = fopen(filename, "a");
}

void log_close(void) {
    if (log_fp) {
        fclose(log_fp);
        log_fp = NULL;
    }
}

void log_message(const char *msg) {
    if (!log_fp) return;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    fprintf(log_fp, "[%s] %s\n", time_str, msg);
    fflush(log_fp);
}

HWND find_window(const char *class_name, const char *window_name) {
    HWND hwnd = FindWindowA(class_name, window_name);
    if (hwnd) {
        char buf[256];
        snprintf(buf, sizeof(buf), "Found window: %s", window_name);
        log_message(buf);
    }
    return hwnd;
}

HWND get_child_window(HWND hwnd) {
    if (!hwnd) return NULL;
    return GetWindow(hwnd, GW_CHILD);
}

BOOL get_window_rect(HWND hwnd, RECT *rect) {
    if (!hwnd || !rect) return FALSE;
    return GetWindowRect(hwnd, rect);
}

BOOL set_window_pos_hw(HWND hwnd, int x, int y, int width, int height) {
    if (!hwnd) return FALSE;
    BOOL result = SetWindowPos(hwnd, HWND_TOP, x, y, width, height, 0);
    if (result) {
        char buf[256];
        snprintf(buf, sizeof(buf), "SetWindowPos: %dx%d", width, height);
        log_message(buf);
    }
    return result;
}

BOOL is_process_running(const char *process_name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(snapshot, &pe32)) {
        CloseHandle(snapshot);
        return FALSE;
    }

    do {
        if (stricmp(pe32.szExeFile, process_name) == 0) {
            CloseHandle(snapshot);
            char buf[256];
            snprintf(buf, sizeof(buf), "Process running: %s", process_name);
            log_message(buf);
            return TRUE;
        }
    } while (Process32Next(snapshot, &pe32));

    CloseHandle(snapshot);
    return FALSE;
}

BOOL start_process(const char *path, const char *params) {
    if (!path || !*path) return FALSE;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmdline[MAX_PATH * 2];
    if (params && *params) {
        snprintf(cmdline, sizeof(cmdline), "\"%s\" %s", path, params);
    } else {
        snprintf(cmdline, sizeof(cmdline), "\"%s\"", path);
    }

    BOOL result = CreateProcessA(
        NULL,
        cmdline,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (result) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        char buf[256];
        snprintf(buf, sizeof(buf), "Started process: %s %s", path, params ? params : "");
        log_message(buf);
    } else {
        char buf[256];
        snprintf(buf, sizeof(buf), "Failed to start process: %s", path);
        log_message(buf);
    }

    return result;
}