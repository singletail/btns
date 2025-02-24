#include "ntp.h"

// Dedicated to Morris Day

static const char *URI = NTP_SERVER;
static const char *TZ = TIME_ZONE;

static bool config = false;
static bool busy = false;

void ntp_init(void) {
    bits_wait(BIT_NET_READY);
    if (!config) {
        setenv("TZ", TZ, 1);
        tzset();
        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, URI);
        esp_sntp_set_time_sync_notification_cb(ntp_callback);
        config = true;
    }
    if (!busy) {
        busy = true;
        esp_sntp_init();
    }
}

uint64_t ntp_ms(void) { return esp_timer_get_time() / 1000; }

char *ntp_ts(void) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    return asctime(&timeinfo);
}

void ntp_print(void) {
    if (!bits_bool(BIT_NTP_READY)) {
        WARN("Time is just a construct.");
        return;
    }
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    LOG("Current time: %s", strftime_buf);
}

void ntp_callback(struct timeval *tv) {
    busy = false;
    bits_set(BIT_NTP_READY);
    LOG("ntp sync complete");
    ntp_print();
}
