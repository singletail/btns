#include "strip.h"

#define PI 3.14159265358979323846

#define STRIP_MAX_STRIPS 4
#define STRIP_NUM_STRIPS 3
#define STRIP_NUM_COLORS 3
#define LED_LENGTH 46

strip_t *strips[STRIP_MAX_STRIPS] = {NULL};

const strip_conf_t strip_conf = {
    .num_strips = 3,
    .num_colors = 3,
    .clock_source = SOC_MOD_CLK_APB,
    .resolution_hz = RES_HZ,
    .blk_rmt = 48,      // Minimum required by ESP-IDF
    .blk_dma = 64,      // Keep this reasonably small but larger than minimum
    .queue_rmt = 1,     // Minimal queue size
    .queue_dma = 2,     // Minimal queue size for DMA
};

// buffers
DMA_ATTR static uint8_t strip_buffer_1[LED_LENGTH * STRIP_NUM_COLORS] __attribute__((aligned(16)));
DMA_ATTR static uint8_t strip_buffer_2[LED_LENGTH * STRIP_NUM_COLORS] __attribute__((aligned(16)));
DMA_ATTR static uint8_t strip_buffer_3[LED_LENGTH * STRIP_NUM_COLORS] __attribute__((aligned(16)));

static const rmt_transmit_config_t strip_tx_conf = {
    .loop_count = 0,
};

strip_t *strip_create(int idx, gpio_num_t pin, uint32_t size, bool dma) {
    strip_t *strip = heap_caps_calloc(1, sizeof(strip_t), MALLOC_CAP_DEFAULT);
    if (!strip) {
        ERR("Failed to allocate strip");
        return NULL;
    }

    strip->pin = pin;
    strip->size = size;
    strip->dma = dma;
    // strip->buffer_size = size * STRIP_NUM_COLORS;

    switch (idx) {
        case 0:
            strip->buffer = strip_buffer_1;
            break;
        case 1:
            strip->buffer = strip_buffer_2;
            break;
        case 2:
            strip->buffer = strip_buffer_3;
            break;
        default:
            free(strip);
            return NULL;
    }

    rmt_tx_channel_config_t rmt_config = {
        .gpio_num = pin,
        .clk_src = strip_conf.clock_source,
        .mem_block_symbols = dma ? strip_conf.blk_dma : strip_conf.blk_rmt,
        .resolution_hz = strip_conf.resolution_hz,
        .trans_queue_depth = dma ? strip_conf.queue_dma : strip_conf.queue_rmt,
        .flags.invert_out = false,
        .flags.with_dma = dma,
    };

    esp_err_t ret = rmt_new_tx_channel(&rmt_config, &strip->channel);
    if (ret != ESP_OK) {
        ERR("Failed to create RMT channel for pin %d: %s", pin, esp_err_to_name(ret));
        free(strip);
        return NULL;
    }

    ret = encoder_new(&strip->encoder);
    if (ret != ESP_OK) {
        ERR("Failed to create encoder for strip %d: %d", idx, ret);
        rmt_del_channel(strip->channel);
        free(strip);
        return NULL;
    }

    strips[idx] = strip;  // Store pointer to strip
    return strip;
}

esp_err_t strip_set_pixel_rgb(strip_t *strip, uint32_t pixel, uint8_t r, uint8_t g, uint8_t b) {
    strip->buffer[pixel * strip_conf.num_colors + 0] = g & 0xFF;
    strip->buffer[pixel * strip_conf.num_colors + 1] = r & 0xFF;
    strip->buffer[pixel * strip_conf.num_colors + 2] = b & 0xFF;
    return ESP_OK;
}

esp_err_t strip_set_all_rgb(strip_t *strip, uint8_t r, uint8_t g, uint8_t b) {
        for (uint32_t i = 0; i < strip->size; i++) {
            strip->buffer[i * strip_conf.num_colors + 0] = g & 0xFF;
            strip->buffer[i * strip_conf.num_colors + 1] = r & 0xFF;
            strip->buffer[i * strip_conf.num_colors + 2] = b & 0xFF;
        }
    return ESP_OK;
}

esp_err_t strip_set_range_rgb(
    strip_t *strip, uint32_t pixel_start, uint32_t pixel_end, uint8_t r, uint8_t g, uint8_t b) {
        for (uint32_t i = pixel_start; i < pixel_end; i++) {
            strip->buffer[i * strip_conf.num_colors + 0] = g & 0xFF;
            strip->buffer[i * strip_conf.num_colors + 1] = r & 0xFF;
            strip->buffer[i * strip_conf.num_colors + 2] = b & 0xFF;
        }
    return ESP_OK;
}

esp_err_t strip_enable(strip_t *strip) {
    rmt_enable(strip->channel);
    return ESP_OK;
}

esp_err_t strip_disable(strip_t *strip) {
    rmt_disable(strip->channel);
    return ESP_OK;
}

esp_err_t strip_refresh(strip_t *strip) {
    //rmt_enable(strip->channel);
    rmt_transmit(
        strip->channel, strip->encoder, strip->buffer, strip->size * strip_conf.num_colors,
        &strip_tx_conf);
    rmt_tx_wait_all_done(strip->channel, portMAX_DELAY);
    //rmt_disable(strip->channel);
    return ESP_OK;
}

esp_err_t strip_clear(strip_t *strip) {
    memset(strip->buffer, 0, strip->size * strip_conf.num_colors);
    return ESP_OK;
}


