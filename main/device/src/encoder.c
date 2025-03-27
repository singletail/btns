/*

#include "encoder.h"

static const float TICK_TO_US = RES_HZ / 1000000;    // 80.0; // 80 ticks per microsecond

// WS2812B exact timings:
// T0H = 0.35µs = 28 ticks
// T0L = 0.80µs = 64 ticks
// T1H = 0.70µs = 56 ticks
// T1L = 0.60µs = 48 ticks

static size_t encoder_encode(
    rmt_encoder_t *encoder, rmt_channel_handle_t channel, const void *primary_data,
    size_t data_size, rmt_encode_state_t *ret_state) {
    encoder_obj_t *led_encoder = __containerof(encoder, encoder_obj_t, base);
    rmt_encoder_handle_t bytes_encoder = led_encoder->bytes_encoder;
    rmt_encoder_handle_t copy_encoder = led_encoder->copy_encoder;
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;
        switch (led_encoder->state) {
            case 0:
                encoded_symbols += bytes_encoder->encode(
                    bytes_encoder, channel, primary_data, data_size, &session_state);
                    if (session_state & RMT_ENCODING_COMPLETE) {
                        led_encoder->state = 1;
                }
                    if (session_state & RMT_ENCODING_MEM_FULL) {
                        state |= RMT_ENCODING_MEM_FULL;
                        goto out;
                }

            case 1:    // reset code
                encoded_symbols += copy_encoder->encode(
                    copy_encoder, channel, &led_encoder->reset_code,
                    sizeof(led_encoder->reset_code), &session_state);
                    if (session_state & RMT_ENCODING_COMPLETE) {
                        led_encoder->state = RMT_ENCODING_RESET;
                        state |= RMT_ENCODING_COMPLETE;
                }
                    if (session_state & RMT_ENCODING_MEM_FULL) {
                        state |= RMT_ENCODING_MEM_FULL;
                        goto out;
                }
        }
out:
    *ret_state = state;
    return encoded_symbols;
}

static rmt_encoder_handle_t shared_encoder = NULL;

static esp_err_t encoder_del(rmt_encoder_t *encoder) {
    // Don't delete if other strips are still using it
    return ESP_OK;
}

static esp_err_t encoder_reset(rmt_encoder_t *encoder) {
    encoder_obj_t *led_encoder = __containerof(encoder, encoder_obj_t, base);
    rmt_encoder_reset(led_encoder->bytes_encoder);
    rmt_encoder_reset(led_encoder->copy_encoder);
    led_encoder->state = RMT_ENCODING_RESET;
    return ESP_OK;
}

esp_err_t encoder_new(rmt_encoder_handle_t *ret_encoder) {
    // If shared encoder exists, return it
    if (shared_encoder != NULL) {
        *ret_encoder = shared_encoder;
        return ESP_OK;
    }

    // Create the encoder only once
    encoder_obj_t *led_encoder = rmt_alloc_encoder_mem(sizeof(encoder_obj_t));
    led_encoder->base.encode = encoder_encode;
    led_encoder->base.del = encoder_del;
    led_encoder->base.reset = encoder_reset;

    // Use minimal memory configurations
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = 0.35 * TICK_TO_US,
            .level1 = 0,
            .duration1 = 0.80 * TICK_TO_US,
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = 0.70 * TICK_TO_US,
            .level1 = 0,
            .duration1 = 0.60 * TICK_TO_US,
        },
        .flags.msb_first = 1
    };

    rmt_new_bytes_encoder(&bytes_encoder_config, &led_encoder->bytes_encoder);
    
    // Use minimal copy encoder configuration
    rmt_copy_encoder_config_t copy_encoder_config = {};
    rmt_new_copy_encoder(&copy_encoder_config, &led_encoder->copy_encoder);

    // Use minimal reset code
    uint32_t reset_ticks = 40 * TICK_TO_US / 2;  // Reduced duration
    led_encoder->reset_code = (rmt_symbol_word_t){
        .level0 = 0,
        .duration0 = reset_ticks,
        .level1 = 0,
        .duration1 = reset_ticks,
    };
    
    shared_encoder = &led_encoder->base;
    *ret_encoder = shared_encoder;
    return ESP_OK;
}

*/