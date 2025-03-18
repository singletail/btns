#include "init.h"
#include "seg7.h"
#include "ntp.h"


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

void init() {
    INFO("init()");

    // Create and initialize strip 1
    strip_t *strip_1 = strip_create(PIN_LED_1, LED_LENGTH_1, true, STRIP_IDX_1);
    if (!strip_1) {
        ERR("Failed to create strip 1");
        return;
    }
    strip_enable(strip_1);
    strip_set_all_rgb(strip_1, 255, 0, 0);  // Red
    strip_refresh(strip_1);
    LOG("strip 1 initialized");

    // Create and initialize strip 2
    strip_t *strip_2 = strip_create(PIN_LED_2, LED_LENGTH_2, true, STRIP_IDX_2);
    if (!strip_2) {
        ERR("Failed to create strip 2");
        return;
    }
    strip_enable(strip_2);
    strip_set_all_rgb(strip_2, 0, 255, 0);  // Green
    strip_refresh(strip_2);
    LOG("strip 2 initialized");

    // Create and initialize strip 3
    strip_t *strip_3 = strip_create(PIN_LED_3, LED_LENGTH_3, true, STRIP_IDX_3);
    if (!strip_3) {
        ERR("Failed to create strip 3");
        return;
    }
    strip_enable(strip_3);
    strip_set_all_rgb(strip_3, 0, 0, 255);  // Blue
    strip_refresh(strip_3);
    LOG("strip 3 initialized");

    // Optional: Create tasks for animations
    xTaskCreate(strip_rainbow_task, "rainbow1", 4096, strip_1, 5, NULL);
    xTaskCreate(strip_wave_task, "wave2", 4096, strip_2, 5, NULL);
    xTaskCreate(strip_color_task, "color3", 4096, strip_3, 5, NULL);

#ifdef PIN_SEG7_CLK
    xTaskCreate(clock_task, "clock", 2048, NULL, 2, NULL);
#endif
}