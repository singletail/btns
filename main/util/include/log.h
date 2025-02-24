#ifndef LOG_H
#define LOG_H

#include "includes.h"

#define ERR(fmt, ...) log_draw(col_err, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) log_draw(col_o, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG(fmt, ...) log_draw(col_y, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) log_draw(col_b, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) log_draw(col_v, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define YES(fmt, ...) log_draw(col_g, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)

/*
#define ERR(fmt, ...) log_write("error", __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define EMERG(fmt, ...) log_write("emerg", __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) log_write("warn", __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG(fmt, ...) log_write("log", __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) log_write("info", __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) log_write("debug", __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define YES(fmt, ...) log_write("yes", __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define NO(fmt, ...) log_write("no", __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
*/

// void log_write(const char *level, const char *file, int line, const char *fmt, ...);
void log_draw(const char *col, const char *file, int line, const char *fmt, ...);


#endif
