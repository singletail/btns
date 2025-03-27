#include "ws2812.h"
#include <stdlib.h>
#include <string.h>

#define RMT_RESOLUTION_HZ 10000000 // 10 MHz = 100 ns
#define T0H 350
#define T0L 800
#define T1H 700
#define T1L 600

static esp_err_t ws2812_create_encoder(rmt_encoder_handle_t *ret_encoder) {
    rmt_bytes_encoder_config_t config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = T0H / 100,
            .level1 = 0,
            .duration1 = T0L / 100
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = T1H / 100,
            .level1 = 0,
            .duration1 = T1L / 100
        }
    };
    return rmt_new_bytes_encoder(&config, ret_encoder);
}

esp_err_t led_strip_init(led_strip_t *strip, gpio_num_t gpio, rmt_channel_handle_t channel, uint32_t num_leds) {
    strip->gpio = gpio;
    strip->channel = channel;
    strip->num_leds = num_leds;
    strip->buffer = calloc(num_leds * 3, sizeof(uint8_t)); // GRB format

    ESP_ERROR_CHECK(ws2812_create_encoder(&strip->encoder));
    return ESP_OK;
}

void led_strip_set_pixel(led_strip_t *strip, uint32_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= strip->num_leds) return;
    uint8_t *p = &strip->buffer[index * 3];
    p[0] = g;
    p[1] = r;
    p[2] = b;
}

esp_err_t led_strip_draw(led_strip_t *strip) {
    rmt_transmit_config_t tx_cfg = {
        .loop_count = 0,
        .flags.eot_level = 0
    };

    ESP_ERROR_CHECK(rmt_transmit(strip->channel, strip->encoder, strip->buffer, strip->num_leds * 3, &tx_cfg));
    return rmt_tx_wait_all_done(strip->channel, portMAX_DELAY);
}
