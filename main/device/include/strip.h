#ifndef _STRIP_H
#define _STRIP_H

//#include "includes.h"
#include "encoder.h"

#define RES_HZ 80000000

typedef struct {
    gpio_num_t pin;
    uint32_t size;
    bool dma;
    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
    uint8_t *buffer;
} strip_t;

typedef struct {
    uint8_t num_strips;
    uint8_t num_colors;
    soc_module_clk_t clock_source;
    uint32_t resolution_hz;
    size_t blk_rmt;
    size_t blk_dma;
    size_t queue_rmt;
    size_t queue_dma;
} strip_conf_t;

extern const strip_conf_t strip_conf;

strip_t *strip_create(int idx, gpio_num_t pin, uint32_t size, bool dma);
esp_err_t strip_set_pixel_rgb(strip_t *strip, uint32_t pixel, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_set_all_rgb(strip_t *strip, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_set_range_rgb(
    strip_t *strip, uint32_t pixel_start, uint32_t pixel_end, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_refresh(strip_t *strip);
esp_err_t strip_enable(strip_t *strip);
esp_err_t strip_disable(strip_t *strip);
esp_err_t strip_clear(strip_t *strip);

int32_t strip_xy_to_index(uint32_t x, uint32_t y);
esp_err_t strip_set_pixel_xy(strip_t *strip, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_draw_line(strip_t *strip, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_draw_char(strip_t *strip, int16_t x, int16_t y, char c, uint8_t r, uint8_t g, uint8_t b);

#endif
