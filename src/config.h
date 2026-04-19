#ifndef CONFIG_H
#define CONFIG_H

#define MAX_PATH 260
#define MAX_PARAM 256

typedef struct {
    char war3_path[MAX_PATH];
    char start_param[MAX_PARAM];
    int check_interval_ms;
    int fix_threshold_ms;
    int lock_cursor;
    int show_log;
    char log_file[MAX_PATH];
} Config;

int load_config(const char *filename, Config *cfg);
int save_config(const char *filename, Config *cfg);
void print_config(Config *cfg);

#endif