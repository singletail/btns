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

#define NUM_LEDS 46

static rmt_channel_handle_t create_rmt_channel(gpio_num_t gpio) {
    rmt_tx_channel_config_t cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = gpio,
        .mem_block_symbols = 64,
        .resolution_hz = 10000000,
        .trans_queue_depth = 4,
        .flags.with_dma = true
    };

    rmt_channel_handle_t channel = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&cfg, &channel));
    ESP_ERROR_CHECK(rmt_enable(channel));
    return channel;
}


void init() {
    INFO("init()");
    led_strip_t strip1, strip2, strip3;

    rmt_channel_handle_t ch0 = create_rmt_channel(PIN_LED_1);
    rmt_channel_handle_t ch1 = create_rmt_channel(PIN_LED_2);
    rmt_channel_handle_t ch2 = create_rmt_channel(PIN_LED_3);

    ESP_ERROR_CHECK(led_strip_init(&strip1, GPIO_NUM_2, ch0, NUM_LEDS));
    ESP_ERROR_CHECK(led_strip_init(&strip2, GPIO_NUM_3, ch1, NUM_LEDS));
    ESP_ERROR_CHECK(led_strip_init(&strip3, GPIO_NUM_4, ch2, NUM_LEDS));

    for (int i = 0; i < NUM_LEDS; i++) {
        led_strip_set_pixel(&strip1, i, 255, 0, 0); // red
        led_strip_set_pixel(&strip2, i, 0, 255, 0); // green
        led_strip_set_pixel(&strip3, i, 0, 0, 255); // blue
    }

    led_strip_draw(&strip1);
    led_strip_draw(&strip2);
    led_strip_draw(&strip3);



#ifdef PIN_SEG7_CLK
    xTaskCreate(clock_task, "clock", 2048, NULL, 2, NULL);
#endif
}