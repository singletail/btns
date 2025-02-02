#ifndef _CTRL_TIME_H
#define _CTRL_TIME_H

// #ifdef ENABLE_TIME

#include "include.h"
#include "seg7.h"

typedef struct {
    const char *name;
    uint32_t interval;
    uint64_t start;
    uint64_t stop;
    uint64_t count;
    uint64_t total;
    float avg;
    float fps;
} timex_perf_t;

void time_timex(void);
uint64_t timex_ms(void);
char *timex_ts(void);
void timex_delay(uint32_t us);
void print_timex(void);
void timex_sntp_callback(struct timeval *tv);

timex_perf_t timex_perf_new(const char *name);
void timex_perf_start(timex_perf_t *p);
void timex_perf_stop(timex_perf_t *p);
void timex_perf_report(timex_perf_t *p);

#endif

// #endif