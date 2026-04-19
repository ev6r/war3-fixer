#include "fixer.h"
#include "winapi.h"
#include <stdio.h>
#include <windows.h>

extern int g_show_log;

HWND find_war3_window(void) {
    HWND hwnd;

    hwnd = find_window("Warcraft III", "Warcraft III");
    if (hwnd) return hwnd;

    hwnd = find_window("Black Warcraft III", "Warcraft III");
    if (hwnd) {
        HWND child = get_child_window(hwnd);
        if (child) {
            if (g_show_log) log_message("Found child window for 11 platform");
            return child;
        }
        return hwnd;
    }

    return NULL;
}

int set_borderless_fullscreen(HWND hwnd) {
    if (!hwnd) {
        if (g_show_log) log_message("Window handle is NULL");
        return -1;
    }

    HWND desktop = GetDesktopWindow();
    RECT desktop_rect;
    if (!GetWindowRect(desktop, &desktop_rect)) {
        if (g_show_log) log_message("Failed to get desktop rect");
        return -1;
    }

    int width = desktop_rect.right - desktop_rect.left;
    int height = desktop_rect.bottom - desktop_rect.top;
    int x = 0;
    int y = 0;

    if (g_show_log) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Desktop: %d,%d %dx%d", x, y, width, height);
        log_message(buf);
    }

    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_BORDER | WS_THICKFRAME | WS_CAPTION);
    SetWindowLong(hwnd, GWL_STYLE, style);

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    if (g_show_log) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Setting borderless fullscreen at (%d,%d): %dx%d", x, y, width, height);
        log_message(buf);
    }
    if (!SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height, 0)) {
        if (g_show_log) log_message("Failed to set fullscreen");
        return -1;
    }

    if (g_show_log) log_message("Borderless fullscreen applied (topmost)");
    return 0;
}

int fix_window(HWND hwnd, int threshold_ms) {
    if (!hwnd) {
        if (g_show_log) log_message("Window handle is NULL");
        return -1;
    }

    RECT rect;
    if (!get_window_rect(hwnd, &rect)) {
        if (g_show_log) log_message("Failed to get window rect");
        return -1;
    }

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int x = rect.left;
    int y = rect.top;

    if (g_show_log) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Current window size: %dx%d at (%d,%d)", width, height, x, y);
        log_message(buf);
    }

    if (g_show_log) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Setting window size: %dx%d (width+1)", width + 1, height);
        log_message(buf);
    }
    if (!SetWindowPos(hwnd, HWND_TOP, x, y, width + 1, height, 0)) {
        if (g_show_log) log_message("Failed to set window size (width+1)");
        return -1;
    }

    Sleep(threshold_ms);

    if (g_show_log) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Restoring window size: %dx%d", width, height);
        log_message(buf);
    }
    if (!SetWindowPos(hwnd, HWND_TOP, x, y, width, height, 0)) {
        if (g_show_log) log_message("Failed to restore window size");
        return -1;
    }

    if (g_show_log) log_message("Window fix completed");
    return 0;
}

int lock_cursor(HWND hwnd) {
    if (!hwnd) {
        return -1;
    }

    HWND foreground = GetForegroundWindow();
    if (foreground != hwnd) {
        HWND war35211 = find_window("Black Warcraft III", "Warcraft III");
        if (foreground == war35211 && war35211 != NULL) {
            foreground = get_child_window(war35211);
        }
    }

    if (foreground == hwnd) {
        RECT rect;
        if (get_window_rect(hwnd, &rect)) {
            if (rect.top == -32000) {
                return -1;
            }
            ClipCursor(&rect);
            if (g_show_log) log_message("Cursor locked to window rect");
        }
    }
    return 0;
}

int unlock_cursor(void) {
    ClipCursor(NULL);
    if (g_show_log) log_message("Cursor unlocked");
    return 0;
}

int is_window_foreground(HWND hwnd) {
    if (!hwnd) return 0;

    HWND foreground = GetForegroundWindow();

    if (g_show_log) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Foreground: %p, War3: %p", (void*)foreground, (void*)hwnd);
        log_message(buf);
    }

    if (foreground == hwnd) {
        if (g_show_log) log_message("Direct match: foreground == war3 window");
        return 1;
    }

    HWND war35211 = find_window("Black Warcraft III", "Warcraft III");
    if (war35211 && foreground == war35211) {
        HWND child = get_child_window(war35211);
        if (g_show_log) {
            char buf[128];
            snprintf(buf, sizeof(buf), "11 Platform detected, child window: %p", (void*)child);
            log_message(buf);
        }
        if (child == hwnd) {
            return 1;
        }
    }

    if (g_show_log) log_message("War3 is NOT in foreground");
    return 0;
}

int set_window_normal(HWND hwnd) {
    if (!hwnd) {
        return -1;
    }

    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    if (g_show_log) log_message("Window removed from topmost");
    return 0;
}