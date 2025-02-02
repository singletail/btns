#include "log.h"

#define LOG_BUF_SIZE 512
static char log_buffer[LOG_BUF_SIZE];

static char log_color[32];
static char log_lvl[32];
static char log_basename[32];
static char log_symbol[32];

void log_get_col(const char *level, char *output) {
    strncpy(output, sym_col(level), 32);
}

void log_get_lvl(const char *level, char *output) {
    strncpy(output, sym_lvl(level), 32);
}

void log_get_symbol(const char *filename, char *output) {
    strncpy(output, sym(filename), 32);
}

void log_get_ts(char *output) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    snprintf(output, 32, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min,
        timeinfo.tm_sec);
}

void log_get_basename(const char *path, char *output) {
    const char *name = strrchr(path, '/');
    name = name ? name + 1 : path;
    const char *dot = strchr(name, '.');
    strncpy(output, name, dot ? (size_t)(dot - name) : strlen(name));
    output[dot ? (size_t)(dot - name) : strlen(name)] = '\0';
}

void log_write(
    const char *level, const char *file, int line, const char *fmt, ...) {
    log_get_col(level, log_color);
    log_get_lvl(level, log_lvl);
    // log_get_ts(log_ts);
    log_get_basename(file, log_basename);
    log_get_symbol(log_basename, log_symbol);
    int len = snprintf(log_buffer, LOG_BUF_SIZE, "%s[%s%s %s:%d] ", log_color,
        log_lvl, log_symbol, log_basename, line);

    va_list args;
    va_start(args, fmt);
    len += vsnprintf(log_buffer + len, LOG_BUF_SIZE - len, fmt, args);
    va_end(args);
    len += snprintf(log_buffer + len, LOG_BUF_SIZE - len, "%s\n", "\033[0m");
    fwrite(log_buffer, 1, len, stdout);

#ifdef LOG_TO_FILE
    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        fwrite(log_buffer, 1, len, f);
        fclose(f);
    }
#endif
}