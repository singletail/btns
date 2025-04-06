#pragma once
#include "includes.h"

void test_strips(void);
void print_rmt_status(void);
/*
#include "includes.h"
#include "driver/rmt_tx.h"
#include "esp_err.h"
#include "driver/gpio.h"

typedef struct {
    uint8_t *buffer;          // RGB data (3 bytes per LED)
    uint32_t length;          // Number of LEDs
    gpio_num_t pin;           // GPIO pin
    rmt_channel_handle_t channel; // RMT channel
    bool enabled;             // Enabled state
} strip_t;

esp_err_t ws2812_encoder_init(void);
strip_t* ws2812_strip_create(gpio_num_t pin, uint16_t length);
void ws2812_set_pixel(strip_t *strip, uint16_t index, uint8_t r, uint8_t g, uint8_t b);
esp_err_t ws2812_update(strip_t *strip);
void ws2812_strip_delete(strip_t *strip);

*/