#include "log.h"

#define LOG_BUF_SIZE 512
static char log_buffer[LOG_BUF_SIZE];
static char log_color[32];
static char log_lvl[32];
static char log_basename[32];
static char log_symbol[32];

void log_get_symbol(const char *filename, char *output) { strncpy(output, sym(filename), 32); }

void log_get_basename(const char *path, char *output) {
    const char *name = strrchr(path, '/');
    name = name ? name + 1 : path;
    const char *dot = strchr(name, '.');
    strncpy(output, name, dot ? (size_t)(dot - name) : strlen(name));
    output[dot ? (size_t)(dot - name) : strlen(name)] = '\0';
}

void log_draw(const char *col, const char *file, int line, const char *fmt, ...) {
    strncpy(log_color, col, sizeof(log_color) - 1);
    log_color[sizeof(log_color) - 1] = '\0';  // Ensure null-termination

    log_get_basename(file, log_basename);
    log_get_symbol(log_basename, log_symbol);

    int len = snprintf(
        log_buffer, LOG_BUF_SIZE, "%s[%s%s %s:%d] ", log_color, log_lvl, log_symbol, log_basename, line);

    va_list args;
    va_start(args, fmt);
    len += vsnprintf(log_buffer + len, LOG_BUF_SIZE - len, fmt, args);
    va_end(args);

    snprintf(log_buffer + len, LOG_BUF_SIZE - len, "%s\n", col_reset);
    fwrite(log_buffer, 1, strlen(log_buffer), stdout);

#ifdef LOG_TO_FILE
    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        fwrite(log_buffer, 1, strlen(log_buffer), f);
        fclose(f);
    }
#endif
}