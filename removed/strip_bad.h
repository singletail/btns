#ifndef STRIP_H
#define STRIP_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_types.h"
#include "driver/gpio.h"

#define RES_HZ 80000000

typedef struct strip_t {
    gpio_num_t pin;
    uint32_t size;
    bool dma;
    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
    uint8_t *buffer;
} strip_t;

typedef struct {
    uint32_t resolution_hz;
    uint32_t size;
    bool dma;
} strip_conf_t;

extern const strip_conf_t strip_conf;

strip_t *strip_create(int idx, gpio_num_t pin, uint32_t size, bool dma);
esp_err_t strip_set_pixel_rgb(strip_t *strip, uint32_t index, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_set_all_rgb(strip_t *strip, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_set_range_rgb(
    strip_t *strip, uint32_t pixel_start, uint32_t pixel_end, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_refresh(strip_t *strip);
esp_err_t strip_enable(strip_t *strip);
esp_err_t strip_disable(strip_t *strip);
esp_err_t strip_clear(strip_t *strip);
esp_err_t strip_delete(strip_t *strip);

#endif // STRIP_H
