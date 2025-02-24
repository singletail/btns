#ifndef _PERF_H
#define _PERF_H

#include "ntp.h"
#include <time.h>

typedef struct {
    const char *name;
    uint32_t interval;
    uint64_t start;
    uint64_t stop;
    uint64_t count;
    uint64_t total;
    float avg;
    float fps;
} perf_t;


perf_t perf_new(const char *name);
void perf_start(perf_t *p);
void perf_stop(perf_t *p);
void perf_report(perf_t *p);

#endif
