#include "ws2812.h"
#include "driver/rmt_encoder.h"

#define RMT_RESOLUTION_HZ 10000000 // 10 MHz = 100 ns
#define T0H 350
#define T0L 800
#define T1H 700
#define T1L 600

static rmt_encoder_handle_t ws2812_encoder = NULL;

static esp_err_t ws2812_create_encoder(rmt_encoder_handle_t *ret_encoder) {
    INFO("ws2812_create_encoder()");
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



// Initialize encoder
esp_err_t ws2812_encoder_init(void) {
    INFO("ws2812_encoder_init()");
    if (ws2812_encoder != NULL) {
        WARN("Encoder already exists");
        return ESP_OK;  // Already initialized
    }
    
    // WS2812B timing
    return ws2812_create_encoder(&ws2812_encoder);
}

// Create a strip
strip_t* ws2812_strip_create(gpio_num_t pin, uint16_t length) {
    strip_t *strip = calloc(1, sizeof(strip_t));
    if (!strip) return NULL;
    
    strip->pin = pin;
    strip->length = length;
    strip->buffer = calloc(length * 3, sizeof(uint8_t));
    if (!strip->buffer) {
        free(strip);
        return NULL;
    }
    
    // Ensure encoder is initialized
    if (ws2812_encoder_init() != ESP_OK) {
        free(strip->buffer);
        free(strip);
        return NULL;
    }
    
    // Configure minimal RMT channel for DMA
    rmt_tx_channel_config_t tx_config = {
        .clk_src = RMT_CLK_SRC_APB,  // 80MHz clock
        .gpio_num = pin,
        .mem_block_symbols = 64,      // Minimal memory blocks
        .resolution_hz = 10000000,    // 10MHz resolution (0.1μs precision)
        .trans_queue_depth = 4,       // Small queue
        .flags.invert_out = false,
        .flags.with_dma = false
    };
    
    INFO("Creating RMT channel for pin %d", pin);

    esp_err_t ret = rmt_new_tx_channel(&tx_config, &strip->channel);
    if (ret != ESP_OK) {
        INFO("Failed to create RMT channel: %s", esp_err_to_name(ret));
        free(strip->buffer);
        free(strip);
        return NULL;
    }
    INFO("RMT channel created successfully");
    return strip;
}

// Set RGB for a single LED
void ws2812_set_pixel(strip_t *strip, uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= strip->length) return;
    
    // WS2812B expects GRB order
    uint16_t start = index * 3;
    strip->buffer[start] = g;
    strip->buffer[start + 1] = r;
    strip->buffer[start + 2] = b;
}

// Update strip with current buffer
esp_err_t ws2812_update(strip_t *strip) {
    if (!strip->enabled) {
        rmt_enable(strip->channel);
        strip->enabled = true;
    }
    
    rmt_transmit_config_t tx_config = {
        .loop_count = 0
    };
    
    return rmt_transmit(strip->channel, ws2812_encoder, 
                        strip->buffer, strip->length * 3, &tx_config);
}

void ws2812_strip_delete(strip_t *strip) {
    if (!strip) return;
    
    if (strip->enabled) {
        rmt_disable(strip->channel);
    }
    
    rmt_del_channel(strip->channel);
    free(strip->buffer);
    free(strip);
}

/*
#include <stdlib.h>
#include <string.h>





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
*/