#include "ws2812.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_tx.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "ws2812_rmt";


//   WORKING BUT NOT IDEAL -- KEEP FOR REFERENCE

#define NUM_CHANNELS 4
#define NUM_LEDS_PER_STRIP 46
#define RMT_CLK_HZ 10 * 1000 * 1000  // 10 MHz = 100ns resolution
#define WS2812_RESET_US 50

typedef struct {
    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
    gpio_num_t gpio;
} led_strip_t;

static led_strip_t strips[NUM_CHANNELS] = {
    { .gpio = GPIO_NUM_4 },
    { .gpio = GPIO_NUM_5 },
    { .gpio = GPIO_NUM_6 },
    { .gpio = GPIO_NUM_48 },
};


static void init_strips()
{
    for (int i = 0; i < NUM_CHANNELS; i++) {
        ESP_LOGI(TAG, "Initializing strip %d on GPIO %d", i, strips[i].gpio);

        // 1. Allocate RMT TX channel
        rmt_tx_channel_config_t tx_config = {
            .clk_src = RMT_CLK_SRC_DEFAULT,
            .gpio_num = strips[i].gpio,
            .mem_block_symbols = 48,
            .resolution_hz = RMT_CLK_HZ,
            .trans_queue_depth = 4,
            .flags.with_dma = false
        };
        ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_config, &strips[i].channel));

        // 2. Create bytes encoder for WS2812 protocol
        rmt_bytes_encoder_config_t encoder_config = {
            .bit0 = {
                .duration0 = 3, .level0 = 1,  // T0H = 300ns
                .duration1 = 9, .level1 = 0   // T0L = 900ns
            },
            .bit1 = {
                .duration0 = 8, .level0 = 1,  // T1H = 800ns
                .duration1 = 4, .level1 = 0   // T1L = 400ns
            },
            .flags.msb_first = 1
        };
        ESP_ERROR_CHECK(rmt_new_bytes_encoder(&encoder_config, &strips[i].encoder));

        // 3. Enable TX channel
        ESP_ERROR_CHECK(rmt_enable(strips[i].channel));
    }
}

static void send_color(led_strip_t *strip, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t grb[3] = { green, red, blue };

    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
        .flags.eot_level = 0, // WS2812 requires low at end
    };

    ESP_ERROR_CHECK(rmt_transmit(strip->channel, strip->encoder, grb, sizeof(grb), &tx_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(strip->channel, pdMS_TO_TICKS(100)));

    // WS2812 reset: low for at least 50us
    ets_delay_us(WS2812_RESET_US);
}

void test_strips(void)
{
    INFO("calling init_strips()");
    init_strips();

    INFO("calling send_color()");
    while (1) {
        for (int i = 0; i < NUM_CHANNELS; i++) {
            // Display red on GPIO2, green on GPIO3, blue on GPIO4
            send_color(&strips[i],
                0x10 * (i == 0),
                0x10 * (i == 1),
                0x10 * (i == 2));
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}