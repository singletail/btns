#include "perf.h"

perf_t perf_new(const char *name) {
    perf_t p;
    p.name = name;
    p.interval = 1000;
    p.start = 0;
    p.stop = 0;
    p.count = 0;
    p.total = 0;
    p.avg = 0.0;
    p.fps = 0.0;
    return p;
}

void perf_start(perf_t *p) { p->start = ntp_ms(); }

void perf_stop(perf_t *p) {
    p->stop = ntp_ms();
    p->total += p->stop - p->start;
    p->count++;
        if (p->count >= p->interval) {
            perf_report(p);
            p->count = 0;
            p->total = 0;
    }
}

void perf_report(perf_t *p) {
    p->avg = (p->total / 100.0) / 1000000.0;    // average seconds per frame
    p->fps = (1.0f / p->avg) / 100;             // frames per second
    printf("AVG: %f sec/frame, FPS: %f\n", p->avg, p->fps);
}
