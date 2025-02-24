#include "strip.h"


#define PI 3.14159265358979323846

#define STRIP_MAX_STRIPS 4
#define STRIP_NUM_STRIPS 1
#define STRIP_NUM_COLORS 3

// set these in device.h
#define STRIP_LENGTH_1 LED_LENGTH_1
#define STRIP_PIN_1 PIN_LED_1

strip_t strips[STRIP_MAX_STRIPS];

const strip_conf_t strip_conf = {
    .num_strips = STRIP_NUM_STRIPS,
    .num_colors = STRIP_NUM_COLORS,
    .clock_source = SOC_MOD_CLK_APB,
    .resolution_hz = RES_HZ,
    .blk_rmt = 48,
    .blk_dma = 1024,
    .queue_rmt = 4,
    .queue_dma = 8,
};

DMA_ATTR static uint8_t strip_buffer_main[STRIP_LENGTH_1 * STRIP_NUM_COLORS]
    __attribute__((aligned(16)));
DMA_ATTR static uint8_t strip_buffer_main_calc[STRIP_LENGTH_1 * STRIP_NUM_COLORS]
    __attribute__((aligned(16)));
#define STRIP_BUFFER_MAIN strip_buffer_main
#define STRIP_BUFFER_MAIN_CALC strip_buffer_main_calc

static const rmt_transmit_config_t strip_tx_conf = {
    .loop_count = 0,
};

strip_t *strip_create(gpio_num_t pin, uint32_t size, bool dma, strip_type_t type) {
    strip_t *strip = heap_caps_calloc(1, sizeof(strip_t), MALLOC_CAP_DEFAULT);
    strip->type = STRIP_TYPE_MAIN;
    strip->pin = pin;
    strip->size = size;
    strip->dma = dma;
    strip->buffer_size = size * STRIP_NUM_COLORS;
    strip->buffer_dirty = false;

    strip->buffer = STRIP_BUFFER_MAIN;
    strip->calc_buffer = STRIP_BUFFER_MAIN_CALC;

    rmt_tx_channel_config_t rmt_config = {
        .gpio_num = pin,
        .clk_src = strip_conf.clock_source,
        .mem_block_symbols = dma ? strip_conf.blk_dma : strip_conf.blk_rmt,
        .resolution_hz = strip_conf.resolution_hz,
        .trans_queue_depth = dma ? strip_conf.queue_dma : strip_conf.queue_rmt,
        .flags.invert_out = false,
        .flags.with_dma = dma,
    };
    rmt_new_tx_channel(&rmt_config, &strip->channel);
    encoder_new(&strip->encoder);
    strips[type] = *strip;
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
        if (!strip->buffer_dirty) {
            return ESP_OK;    // Skip refresh if no changes
    }

    rmt_enable(strip->channel);
    rmt_transmit(
        strip->channel, strip->encoder, strip->buffer, strip->size * strip_conf.num_colors,
        &strip_tx_conf);
    rmt_tx_wait_all_done(strip->channel, portMAX_DELAY);
    rmt_disable(strip->channel);

    strip->buffer_dirty = false;
    return ESP_OK;
}

esp_err_t strip_clear(strip_t *strip) {
    memset(strip->buffer, 0, strip->size * strip_conf.num_colors);
    return ESP_OK;
}

esp_err_t strip_del(strip_t *strip) {
    rmt_del_encoder(strip->encoder);
    rmt_del_channel(strip->channel);
    free(strip);
    return ESP_OK;
}

void strip_cycle_task(void *arg) {
    LOG("leds_cycle_task()");

    strip_t *strip = (strip_t *)arg;
    perf_t perf = perf_new("strip_cycle");
    uint8_t cycle_r = 0;
    uint8_t cycle_g = 0;
    uint8_t cycle_b = 0;
    uint8_t cycle_mode = 0;
    uint8_t cycle_step = 1;
    uint8_t cycle_max = 255;

    // seg7_t *led = seg7_new(PIN_SEG7_CLK, PIN_SEG7_DIO);

        while (true) {
            // perf_start(&perf);
                if (cycle_mode == 0) {
                        if (cycle_r < cycle_max) {
                            cycle_r = cycle_r + cycle_step;
                        } else {
                            cycle_mode = 1;
                        }
                } else if (cycle_mode == 1) {
                        if (cycle_r > 0) {
                            cycle_r = cycle_r - cycle_step;
                        } else {
                            cycle_mode = 2;
                        }
                } else if (cycle_mode == 2) {
                        if (cycle_g < cycle_max) {
                            cycle_g = cycle_g + cycle_step;
                        } else {
                            cycle_mode = 3;
                        }
                } else if (cycle_mode == 3) {
                        if (cycle_g > 0) {
                            cycle_g = cycle_g - cycle_step;
                        } else {
                            cycle_mode = 4;
                        }
                } else if (cycle_mode == 4) {
                        if (cycle_b < cycle_max) {
                            cycle_b = cycle_b + cycle_step;
                        } else {
                            cycle_mode = 5;
                        }
                } else if (cycle_mode == 5) {
                        if (cycle_b > 0) {
                            cycle_b = cycle_b - cycle_step;
                        } else {
                            cycle_mode = 6;
                        }
                } else if (cycle_mode == 6) {
                        if (cycle_r < cycle_max) {
                            cycle_r = cycle_r + cycle_step;
                            cycle_b = cycle_r;
                            cycle_g = cycle_r;
                        } else {
                            cycle_mode = 7;
                        }
                } else if (cycle_mode == 7) {
                        if (cycle_r > 0) {
                            cycle_r = cycle_r - cycle_step;
                            cycle_b = cycle_r;
                            cycle_g = cycle_r;
                        } else {
                            cycle_r = 0;
                            cycle_b = 0;
                            cycle_g = 0;
                            cycle_mode = 0;
                        }
            }
            strip->buffer_dirty = true;
            strip_set_all_rgb(strip, cycle_r, cycle_g, cycle_b);
            strip_refresh(strip);
            // perf_stop(&perf);
            // seg7_clear(led);
            // seg7_display_number(led, (int)perf.fps);
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
}

void strip_test_task(void *arg) {
    LOG("leds_test_task()");
    strip_t *strip = (strip_t *)arg;

    uint8_t cycle_r = 0;
    uint8_t cycle_g = 0;
    uint8_t cycle_b = 0;
    uint8_t led_counter = 0;

        while (true) {
                if (led_counter == 0) {
                    cycle_r = 0;
                    cycle_g = 255;
                    cycle_b = 0;
                } else if (led_counter == 1) {
                    cycle_r = 255;
                    cycle_g = 0;
                    cycle_b = 0;
                } else if (led_counter == 2) {
                    cycle_r = 0;
                    cycle_g = 0;
                    cycle_b = 255;
                } else if (led_counter == 3) {
                    cycle_r = 0;
                    cycle_g = 0;
                    cycle_b = 0;
            }

            led_counter++;
            if (led_counter > 3) led_counter = 0;
            /*
                leds_t *leds_obj = leds_objs[stripnum];
                for (uint32_t i = 0; i < leds_obj->size; i++) {
                  leds_obj->buffer[i * leds_cols + 0] = cycle_g & 0xFF;
                  leds_obj->buffer[i * leds_cols + 1] = cycle_r & 0xFF;
                  leds_obj->buffer[i * leds_cols + 2] = cycle_b & 0xFF;
                }
                */
            strip_set_all_rgb(strip, cycle_r, cycle_g, cycle_b);
            strip->buffer_dirty = true;
            strip_refresh(strip);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
}

void strip_wave_task(void *arg) {
    LOG("strip_wave_task()");
    strip_t *strip = (strip_t *)arg;

    const float phase_offset = PI / 3.0f;
    float phases[6] = {
        0.0f,
        phase_offset,
        2.0f * phase_offset,
        3.0f * phase_offset,
        4.0f * phase_offset,
        5.0f * phase_offset};
    const float phase_inc = 0.04f;
    const float wave_len = 36.0f;

    // Use both buffers for double buffering
    uint8_t *front_buffer = strip->buffer;
    uint8_t *back_buffer = strip->calc_buffer;

        while (true) {

                // Calculate next frame into back buffer while current frame is
                // displaying
                for (uint32_t i = 0; i < strip->size; i++) {
                    float waves[6];
                    uint8_t levels[6];
                        for (int w = 0; w < 6; w++) {
                            waves[w] = sinf((i * (2.0f * PI) / wave_len) + phases[w]);
                            levels[w] = (uint8_t)((waves[w] + 1.0f) * 50.0f);
                        }

                    uint8_t r = levels[0] + levels[1] / 2 + levels[5] / 2;
                    uint8_t g = levels[2] + levels[1] / 2 + levels[3];
                    uint8_t b = levels[4] + levels[5] / 2 + levels[3] / 2;

                    back_buffer[i * strip_conf.num_colors + 0] = g;
                    back_buffer[i * strip_conf.num_colors + 1] = r;
                    back_buffer[i * strip_conf.num_colors + 2] = b;
                }

            // Swap buffers
            strip->buffer = back_buffer;
            back_buffer = front_buffer;
            front_buffer = strip->buffer;

                // Update phases for next frame
                for (int w = 0; w < 6; w++) {
                    phases[w] += phase_inc;
                    if (phases[w] > 2.0f * PI) phases[w] -= 2.0f * PI;
                }

            strip->buffer_dirty = true;
            strip_refresh(strip);
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
}
// #endif

void strip_chase_task(void *arg) {
    LOG("strip_chase_task()");
    strip_t *strip = (strip_t *)arg;
    perf_t perf = perf_new("strip_chase");

    // RGB colors array - 12 colors × 3 components each
    const uint8_t thing_colors[20][3] = {
        {16, 0, 0},      // Dark red
        {32, 0, 0},      // Dark red
        {64, 0, 0},      // Medium red
        {128, 0, 0},     // Bright red
        {192, 0, 0},     // Full red
        {255, 0, 0},     // Full red
        {255, 64, 0},    // Red-orange
        {255, 32, 0},    // Orange
        {128, 16, 0},    // Bright orange
        {92, 0, 0},      // Yellow
        {64, 0, 0},      // Yellow-green
        {32, 0, 0},   {24, 0, 0}, {20, 0, 0}, {18, 0, 0}, {16, 0, 0},
        {14, 0, 0},   {12, 0, 0}, {8, 0, 0},  {4, 0, 0},
    };

    int8_t thing_length = 20;
    int16_t thing_pos = 0;
    int8_t thing_step = 1;
    int32_t thing_max = strip->size;
    int32_t col_idx = 0;

    // bk

    uint8_t cycle_r = 0;
    uint8_t cycle_g = 0;
    uint8_t cycle_b = 0;
    uint8_t led_counter = 0;

        while (true) {
            perf_start(&perf);

            thing_pos = thing_pos + thing_step;

                if (thing_pos > thing_max) {
                    thing_step = -1;
                } else if (thing_pos < 0) {
                    thing_step = 1;
            }

            // Clear previous frame
            // strip_set_all_rgb(strip, 0, 0, 0);

                if (led_counter == 0) {
                    cycle_r = 0;
                    cycle_g = 255;
                    cycle_b = 0;
                } else if (led_counter == 1) {
                    cycle_r = 255;
                    cycle_g = 0;
                    cycle_b = 0;
                } else if (led_counter == 2) {
                    cycle_r = 0;
                    cycle_g = 0;
                    cycle_b = 255;
                } else if (led_counter == 3) {
                    cycle_r = 0;
                    cycle_g = 0;
                    cycle_b = 0;
            }

            led_counter++;
            if (led_counter > 3) led_counter = 0;

                for (int i = 0; i < strip->size; i++) {

                    // i = thing_pos;

                        if (i < thing_pos || i > (thing_pos + thing_length)) {
                            strip_set_pixel_rgb(strip, i, cycle_r, cycle_g, cycle_b);
                        } else {
                            col_idx = i - thing_pos;
                            strip_set_pixel_rgb(
                                strip, i, thing_colors[col_idx][0], thing_colors[col_idx][1],
                                thing_colors[col_idx][2]);
                        }
                }
            /*
            while (i < (thing_pos + thing_length)) {
                if (thing_step < 0) {
                    col_idx = i - thing_pos;
                } else {
                    col_idx = (thing_length - 1) - (i - thing_pos);
                }

                if (i > -1 && i < thing_max) {
                    // strip_set_pixel_rgb(strip, i, thing_colors[col_idx][0],
                    //     thing_colors[col_idx][1], thing_colors[col_idx][2]);
                }
                i++;
                */
            strip->buffer_dirty = true;
            strip_refresh(strip);
            perf_stop(&perf);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
}
