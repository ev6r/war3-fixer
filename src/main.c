#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "config.h"
#include "fixer.h"
#include "winapi.h"

int g_show_log = 0;
char g_log_file[MAX_PATH] = "fixer.log";

static volatile int g_running = 1;

static BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType) {
    if (dwCtrlType == CTRL_C_EVENT) {
        printf("\n[INFO] Received Ctrl+C, exiting...\n");
        g_running = 0;
        return TRUE;
    }
    return FALSE;
}

static void extract_filename(const char *path, char *filename, int size) {
    const char *p = strrchr(path, '\\');
    const char *src = p ? p + 1 : path;
    int len = (int)strlen(src);
    int copy_len = len < size - 1 ? len : size - 1;
    memcpy(filename, src, copy_len);
    filename[copy_len] = '\0';
}

static int file_exists(const char *path) {
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

static void find_war3_in_current_dir(char *path, int size) {
    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA("war3.exe", &find_data);
    if (hFind != INVALID_HANDLE_VALUE) {
        GetCurrentDirectoryA(size, path);
        strcat(path, "\\war3.exe");
        FindClose(hFind);
    } else {
        path[0] = '\0';
    }
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--showlog") == 0 || strcmp(argv[i], "-v") == 0) {
            g_show_log = 1;
        }
    }

    printf("=== War3 Window Fixer ===\n");

    Config cfg;
    if (load_config("config.ini", &cfg) != 0) {
        printf("[WARN] Failed to load config.ini, using defaults\n");
        memset(&cfg, 0, sizeof(cfg));
        strcpy(cfg.war3_path, "war3.exe");
        strcpy(cfg.start_param, "-window");
        cfg.check_interval_ms = 10000;
        cfg.fix_threshold_ms = 1000;
        cfg.lock_cursor = 1;
        cfg.show_log = 0;
        strcpy(cfg.log_file, "fixer.log");
        save_config("config.ini", &cfg);
        printf("[INFO] Created default config.ini\n");
    }

    if (!file_exists(cfg.war3_path)) {
        printf("[WARN] Configured war3 path not found: %s\n", cfg.war3_path);
        char local_path[MAX_PATH];
        find_war3_in_current_dir(local_path, MAX_PATH);
        if (local_path[0] != '\0') {
            printf("[INFO] Found war3.exe in current directory: %s\n", local_path);
            strncpy(cfg.war3_path, local_path, MAX_PATH - 1);
            cfg.war3_path[MAX_PATH - 1] = '\0';
        } else {
            printf("[WARN] war3.exe not found in current directory\n");
        }
    }

    if (g_show_log) {
        cfg.show_log = 1;
    }
    g_show_log = cfg.show_log;

    if (g_show_log) {
        strcpy(g_log_file, cfg.log_file);
        log_init(g_log_file);
    }

    print_config(&cfg);

    if (!SetConsoleCtrlHandler(console_ctrl_handler, TRUE)) {
        printf("[WARN] Failed to set Ctrl+C handler\n");
    }

    char war3_exe[MAX_PATH];
    extract_filename(cfg.war3_path, war3_exe, MAX_PATH);

    printf("\n[INFO] Checking War3 status...\n");

    if (is_process_running(war3_exe)) {
        printf("[INFO] War3 is already running, applying borderless fullscreen...\n");
        HWND hwnd = find_war3_window();
        if (hwnd) {
            Sleep(500);
            set_borderless_fullscreen(hwnd);
            if (cfg.lock_cursor) {
                lock_cursor(hwnd);
            }
            printf("[INFO] Borderless fullscreen applied\n");
        } else {
            printf("[WARN] War3 process found but window not found\n");
        }
    } else {
        printf("[INFO] War3 is not running, trying to start...\n");
        if (*cfg.war3_path) {
            if (start_process(cfg.war3_path, cfg.start_param)) {
                printf("[INFO] War3 started with params: %s\n", cfg.start_param);
                Sleep(2000);
                HWND hwnd = find_war3_window();
                if (hwnd) {
                    set_borderless_fullscreen(hwnd);
                    if (cfg.lock_cursor) {
                        lock_cursor(hwnd);
                    }
                    printf("[INFO] Borderless fullscreen applied\n");
                }
            } else {
                printf("[WARN] Failed to start War3\n");
            }
        } else {
            printf("[WARN] War3 path not configured\n");
        }
    }

    printf("\n[INFO] Starting monitoring loop (Ctrl+C to exit)...\n\n");

    while (g_running) {
        HWND hwnd = find_war3_window();
        if (hwnd) {
            if (is_window_foreground(hwnd)) {
                set_borderless_fullscreen(hwnd);
                fix_window(hwnd, cfg.fix_threshold_ms);
                if (cfg.lock_cursor) {
                    lock_cursor(hwnd);
                }
            } else {
                set_window_normal(hwnd);
                if (cfg.lock_cursor) {
                    unlock_cursor();
                }
            }
        }

        Sleep(cfg.check_interval_ms);
    }

    if (cfg.lock_cursor) {
        unlock_cursor();
    }

    if (g_show_log) {
        log_close();
    }

    printf("[INFO] Exiting...\n");
    return 0;
}