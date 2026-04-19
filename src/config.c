#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int g_show_log;

static void trim_whitespace(char *str) {
    if (!str) return;
    while (*str == ' ' || *str == '\t') str++;
    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
}

static char *get_value(char *line, const char *key) {
    static char value[512];
    if (strncmp(line, key, strlen(key)) == 0) {
        char *p = line + strlen(key);
        while (*p == ' ' || *p == '=') p++;
        strncpy(value, p, sizeof(value) - 1);
        value[sizeof(value) - 1] = '\0';
        trim_whitespace(value);
        return value;
    }
    return NULL;
}

int load_config(const char *filename, Config *cfg) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return -1;
    }

    memset(cfg, 0, sizeof(Config));
    cfg->check_interval_ms = 10000;
    cfg->fix_threshold_ms = 1000;
    cfg->lock_cursor = 1;
    cfg->show_log = 0;
    strcpy(cfg->log_file, "fixer.log");

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        trim_whitespace(line);
        if (line[0] == ';' || line[0] == '#' || line[0] == '\0') continue;
        if (line[0] == '[') continue;

        char *val;
        if ((val = get_value(line, "path"))) strncpy(cfg->war3_path, val, MAX_PATH - 1);
        else if ((val = get_value(line, "start_param"))) strncpy(cfg->start_param, val, MAX_PARAM - 1);
        else if ((val = get_value(line, "check_interval_ms"))) cfg->check_interval_ms = atoi(val);
        else if ((val = get_value(line, "fix_threshold_ms"))) cfg->fix_threshold_ms = atoi(val);
        else if ((val = get_value(line, "lock_cursor"))) cfg->lock_cursor = atoi(val);
        else if ((val = get_value(line, "show_log"))) cfg->show_log = atoi(val);
        else if ((val = get_value(line, "log_file"))) strncpy(cfg->log_file, val, MAX_PATH - 1);
    }

    fclose(fp);
    return 0;
}

int save_config(const char *filename, Config *cfg) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return -1;

    fprintf(fp, "[War3]\n");
    fprintf(fp, "path=%s\n", cfg->war3_path);
    fprintf(fp, "start_param=%s\n\n", cfg->start_param);
    fprintf(fp, "[General]\n");
    fprintf(fp, "check_interval_ms=%d\n", cfg->check_interval_ms);
    fprintf(fp, "fix_threshold_ms=%d\n", cfg->fix_threshold_ms);
    fprintf(fp, "lock_cursor=%d\n\n", cfg->lock_cursor);
    fprintf(fp, "[Debug]\n");
    fprintf(fp, "show_log=%d\n", cfg->show_log);
    fprintf(fp, "log_file=%s\n", cfg->log_file);

    fclose(fp);
    return 0;
}

void print_config(Config *cfg) {
    printf("=== Configuration ===\n");
    printf("War3 Path: %s\n", cfg->war3_path);
    printf("Start Param: %s\n", cfg->start_param);
    printf("Check Interval: %d ms\n", cfg->check_interval_ms);
    printf("Fix Threshold: %d ms\n", cfg->fix_threshold_ms);
    printf("Lock Cursor: %d\n", cfg->lock_cursor);
    printf("Show Log: %d\n", cfg->show_log);
    printf("Log File: %s\n", cfg->log_file);
    printf("=====================\n");
}