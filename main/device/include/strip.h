#ifndef _STRIP_H
#define _STRIP_H

#include "includes.h"

#define RES_HZ 80000000

typedef enum
{
    STRIP_TYPE_MAIN,
    STRIP_TYPE_STATUS,
} strip_type_t;

typedef struct {
    gpio_num_t pin;
    uint32_t size;
    strip_type_t type;
    bool dma;
    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
    uint8_t *buffer;
    uint8_t *calc_buffer;
    uint32_t buffer_size;
    bool buffer_dirty;
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

strip_t *strip_create(gpio_num_t pin, uint32_t size, bool dma, strip_type_t type);
esp_err_t strip_set_pixel_rgb(strip_t *strip, uint32_t pixel, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_set_all_rgb(strip_t *strip, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_set_range_rgb(
    strip_t *strip, uint32_t pixel_start, uint32_t pixel_end, uint8_t r, uint8_t g, uint8_t b);
esp_err_t strip_refresh(strip_t *strip);
esp_err_t strip_enable(strip_t *strip);
esp_err_t strip_disable(strip_t *strip);
esp_err_t strip_clear(strip_t *strip);
esp_err_t strip_del(strip_t *strip);

void strip_test_task(void *arg);
void strip_cycle_task(void *arg);
void strip_wave_task(void *arg);
void strip_chase_task(void *arg);
void strip_color_task(void *arg);
void strip_rainbow_task(void *arg);

#endif  
