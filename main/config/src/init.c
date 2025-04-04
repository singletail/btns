#include "init.h"
#include "seg7.h"
#include "ntp.h"


#define LED_PIN_1 43
#define LED_PIN_2 6
#define LED_PIN_3 5


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

#ifdef PIN_SEG7_CLK
    xTaskCreate(clock_task, "clock", 2048, NULL, 2, NULL);
#endif

   strip_t *strip = strip_create(0, LED_PIN_1, 46, false);
   strip_t *strip_2 = strip_create(1, LED_PIN_2, 46, false);
   strip_t *strip_3 = strip_create(2, LED_PIN_3, 46, false);
   rmt_enable(strip->channel);
   rmt_enable(strip_2->channel);
   rmt_enable(strip_3->channel);

   const int spacing = 6;  // Fixed spacing between bright LEDs
   int offset = 0;        // Position offset for movement

    while (1) {
        // First set all LEDs to dim
        strip_set_all_rgb(strip, 64, 0, 0);
        strip_set_all_rgb(strip_2, 64, 0, 64);
        strip_set_all_rgb(strip_3, 0, 0, 64);

        // Then set every 6th LED bright, shifted by offset
        for (int i = 0; i < 46; i++) {
                if ((i + offset) % spacing == 0) {
                    strip_set_pixel_rgb(strip, i, 255, 0, 0);
                    strip_set_pixel_rgb(strip_2, i, 255, 0, 255);
                    strip_set_pixel_rgb(strip_3, i, 0, 0, 255);
            }
        }

        strip_refresh(strip);
        strip_refresh(strip_2);
        strip_refresh(strip_3);

        // Move pattern one position
        offset = (offset - 1);
        if (offset < 0) {
            offset = spacing - 1;
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }


    INFO("Done.");













}