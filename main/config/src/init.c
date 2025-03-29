#include "init.h"
#include "seg7.h"
#include "ntp.h"

/*
void print_rmt_status(void) {
    INFO("RMT Channel Status:");
    for (int i = 0; i < SOC_RMT_CHANNELS_PER_GROUP; i++) {
        INFO("Channel %d: %s", i, 
            rmt_acquire_tx_channel(i) == ESP_OK ? "free" : "in use");
        rmt_release_tx_channel(i);
    }
}
*/


#ifdef PIN_SEG7_CLK
static void clock_task(void *arg) {
    LOG("clock_task()");
    
    // Create 7-segment display instance
    seg7_t *display = seg7_new(PIN_SEG7_CLK, PIN_SEG7_DIO);
    if (!display) {
        WARN("Failed to create 7-segment display");
        vTaskDelete(NULL);
        return;
    }
    
    // Set initial brightness
    seg7_set_brightness(display, 6);
    
    time_t now;
    struct tm timeinfo;
    bool colon_state = false;
    uint32_t last_update = 0;
    const uint32_t COLON_BLINK_MS = 500;  // Blink every 500ms
    
    // Wait for NTP sync
    bits_wait(BIT_NTP_READY);
    
    while (1) {
        time(&now);
        localtime_r(&now, &timeinfo);
        
        // Get current time in ms
        uint32_t current_ms = ntp_ms() % 1000;
        
        // Update display only if needed (every 500ms for colon blink)
        if (current_ms - last_update >= COLON_BLINK_MS || current_ms < last_update) {
            last_update = current_ms;
            colon_state = !colon_state;
            
            // Display hours (00-23)
            seg7_display_digit(display, 0, timeinfo.tm_hour / 10);
            seg7_display_digit(display, 1, timeinfo.tm_hour % 10);
            
            // Display minutes with colon
            // seg7_display(display, 1, seg7_display_digit(display, 1, timeinfo.tm_hour % 10) | (colon_state ? 0x80 : 0x00));
            seg7_display_digit(display, 2, timeinfo.tm_min / 10);
            seg7_display_digit(display, 3, timeinfo.tm_min % 10);
        }
        
        // Sleep for ~100ms - short enough to catch time changes but long enough to be efficient
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif

#define NUM_LEDS 46



void init() {
    INFO("init()");

    strip_t *strip1 = ws2812_strip_create(9, 46);
    strip_t *strip2 = ws2812_strip_create(8, 46);
    // strip_t *strip3 = ws2812_strip_create(7, 46);

    if (!strip1) {
        ERR("Failed to create strip");
        return;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        ws2812_set_pixel(strip1, i, 0, 0, 255);
        ws2812_set_pixel(strip2, i, 0, 255, 0);
        // ws2812_set_pixel(strip3, i, 255, 0, 0);
    }

    ws2812_update(strip1);
    ws2812_update(strip2);
    // ws2812_update(strip3);

#ifdef PIN_SEG7_CLK
    xTaskCreate(clock_task, "clock", 2048, NULL, 2, NULL);
#endif
}