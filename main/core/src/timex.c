#include "timex.h"

// Dedicated to Morris Day

static const char *URI = NTP_SERVER;
static const char *TZ = TIME_ZONE;

void time_timex(void) {
    bits_wait(BIT_NET_OK);
    if (bits_chk(BIT_TIME_INIT)) {
        WARN("Time already started()");
        return;
    }
    if (bits_chk(BIT_TIME_BUSY)) {
        WARN("Time already connecting.");
        return;
    }
    bits_on(BIT_TIME_BUSY);
    LOG("ctrl_ntp_init()");
    setenv("TZ", TZ, 1);
    tzset();
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, URI);
    esp_sntp_set_time_sync_notification_cb(timex_sntp_callback);
    esp_sntp_init();
    bits_on(BIT_TIME_INIT);
}

uint64_t timex_ms(void) {
    return esp_timer_get_time() / U_TO_MS;
}

void timex_delay(uint32_t us) {
    ets_delay_us(us);
}

char *timex_ts(void) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    return asctime(&timeinfo);
}

void print_timex(void) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    LOG("Current time: %s", strftime_buf);
}

void timex_sntp_callback(struct timeval *tv) { // sntp callback
    LOG("sntp sync complete");
    bits_on(BIT_TIME_OK);
    bits_off(BIT_TIME_BUSY);
    print_timex();
}

// simple performance monitor

/* usage:
timex_perf_t perf = timex_perf_new("my_function");
timex_perf_start(&perf);
my_function();
timex_perf_stop(&perf);
*/

timex_perf_t timex_perf_new(const char *name) {
    timex_perf_t p;
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

void timex_perf_start(timex_perf_t *p) {
    p->start = timex_ms();
}

void timex_perf_stop(timex_perf_t *p) {
    p->stop = timex_ms();
    p->total += p->stop - p->start;
    p->count++;
    if (p->count >= p->interval) {
        timex_perf_report(p);
        p->count = 0;
        p->total = 0;
    }
}

void timex_perf_report(timex_perf_t *p) {
    p->avg = (p->total / 100.0) / 1000000.0; // average seconds per frame
    p->fps = (1.0f / p->avg) / 100;          // frames per second
    printf("AVG: %f sec/frame, FPS: %f\n", p->avg, p->fps);
}
