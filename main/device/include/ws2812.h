#pragma once

#include "includes.h"
#include "driver/rmt_tx.h"
#include "esp_err.h"
#include "driver/gpio.h"

typedef struct {
    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
    gpio_num_t gpio;
    uint32_t num_leds;
    uint8_t *buffer;  // GRB format
} led_strip_t;

esp_err_t led_strip_init(led_strip_t *strip, gpio_num_t gpio, rmt_channel_handle_t channel, uint32_t num_leds);
esp_err_t led_strip_draw(led_strip_t *strip);
void led_strip_set_pixel(led_strip_t *strip, uint32_t index, uint8_t r, uint8_t g, uint8_t b);

