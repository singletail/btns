#include "strip.h"


#define PI 3.14159265358979323846

#define STRIP_MAX_STRIPS 4
#define STRIP_NUM_STRIPS 3  // Updated for 3 strips
#define STRIP_NUM_COLORS 3

// set these in device.h
// #define STRIP_LENGTH_1 LED_LENGTH_1
// #define STRIP_PIN_1 PIN_LED_1

strip_t *strips[STRIP_MAX_STRIPS] = {NULL};  // Change to array of pointers

const strip_conf_t strip_conf = {
    .num_strips = STRIP_NUM_STRIPS,
    .num_colors = STRIP_NUM_COLORS,
    .clock_source = SOC_MOD_CLK_APB,
    .resolution_hz = RES_HZ,
    .blk_rmt = 32,     // Reduced from 48
    .blk_dma = 128,    // Reduced from 1024
    .queue_rmt = 2,    // Reduced from 4
    .queue_dma = 4,    // Reduced from 8
};

#ifdef LED_LENGTH_1
DMA_ATTR static uint8_t strip_buffer_1[LED_LENGTH_1 * STRIP_NUM_COLORS] __attribute__((aligned(16)));
DMA_ATTR static uint8_t strip_calc_1[LED_LENGTH_1 * STRIP_NUM_COLORS] __attribute__((aligned(16)));
#define STRIP_BUFFER_1 strip_buffer_1
#define STRIP_CALC_1 strip_calc_1
#endif

#ifdef LED_LENGTH_2
DMA_ATTR static uint8_t strip_buffer_2[LED_LENGTH_2 * STRIP_NUM_COLORS] __attribute__((aligned(16)));
DMA_ATTR static uint8_t strip_calc_2[LED_LENGTH_2 * STRIP_NUM_COLORS] __attribute__((aligned(16)));
#define STRIP_BUFFER_2 strip_buffer_2
#define STRIP_CALC_2 strip_calc_2
#endif

#ifdef LED_LENGTH_3
DMA_ATTR static uint8_t strip_buffer_3[LED_LENGTH_3 * STRIP_NUM_COLORS] __attribute__((aligned(16)));
DMA_ATTR static uint8_t strip_calc_3[LED_LENGTH_3 * STRIP_NUM_COLORS] __attribute__((aligned(16)));
#define STRIP_BUFFER_3 strip_buffer_3
#define STRIP_CALC_3 strip_calc_3
#endif

static const rmt_transmit_config_t strip_tx_conf = {
    .loop_count = 0,
};

strip_t *strip_create(gpio_num_t pin, uint32_t size, bool dma, strip_type_t type) {
    if (type >= STRIP_MAX_STRIPS || strips[type] != NULL) {
        return NULL;  // Invalid type or strip already exists
    }

    strip_t *strip = heap_caps_calloc(1, sizeof(strip_t), MALLOC_CAP_DEFAULT);
    if (!strip) {
        return NULL;
    }

    strip->type = type;
    strip->pin = pin;
    strip->size = size;
    strip->dma = dma;
    strip->buffer_size = size * STRIP_NUM_COLORS;
    strip->buffer_dirty = false;

    // Assign the correct buffer pair based on type
    switch (type) {
        case STRIP_IDX_1:
            strip->buffer = STRIP_BUFFER_1;
            strip->calc_buffer = STRIP_CALC_1;
            break;
        case STRIP_IDX_2:
            strip->buffer = STRIP_BUFFER_2;
            strip->calc_buffer = STRIP_CALC_2;
            break;
        case STRIP_IDX_3:
            strip->buffer = STRIP_BUFFER_3;
            strip->calc_buffer = STRIP_CALC_3;
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
        ERR("Failed to create encoder for strip %d: %d", type, ret);
        rmt_del_channel(strip->channel);
        free(strip);
        return NULL;
    }

    strips[type] = strip;  // Store pointer to strip
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
    rmt_enable(strip->channel);
    rmt_transmit(
        strip->channel, strip->encoder, strip->buffer, strip->size * strip_conf.num_colors,
        &strip_tx_conf);
    rmt_tx_wait_all_done(strip->channel, portMAX_DELAY);
    rmt_disable(strip->channel);
    return ESP_OK;
}

esp_err_t strip_clear(strip_t *strip) {
    memset(strip->buffer, 0, strip->size * strip_conf.num_colors);
    return ESP_OK;
}

esp_err_t strip_del(strip_t *strip) {
    if (!strip) {
        return ESP_ERR_INVALID_ARG;
    }

    if (strip->type < STRIP_MAX_STRIPS) {
        strips[strip->type] = NULL;  // Clear from array
    }

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
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
}

void strip_color_task(void *arg) {
    LOG("strip_color_task()");
    strip_t *strip = (strip_t *)arg;

    int r = 255;
    int g = 0;
    int b = 0;

    int grad = 64;
    int temp = 0;
    int max = LED_LENGTH_1;

        while (true) {

            for (int i = 0; i < max; i++) {
                r = 0;
                g = 0;
                b = 0;

                // red
                if (i < (max/2) - (grad/2)) {
                    // First half: r=255
                    r = 255;
                } else if (i < (max/2) + (grad/2)) {
                    // Transition to fourth sixth (255 to 0)
                    temp = i - ((max/2) - (grad/2));
                    r = 255 - ((temp * 255) / grad);
                } else if (i < (5*max/6) - (grad/2)) {
                    // Fourth and fifth sixth: r=0
                    r = 0;
                } else if (i < (5*max/6) + (grad/2)) {
                    // Transition to sixth sixth (0 to 255)
                    temp = i - ((5*max/6) - (grad/2));
                    r = (temp * 255) / grad;
                } else {
                    // Sixth sixth: r=255
                    r = 255;
                }

                if (i < (max/6) - (grad/2)) {
                    // First sixth: g=0
                    g = 0;
                } else if (i < (max/6) + (grad/2)) {
                    // Transition to second sixth (0 to 128)
                    temp = i - ((max/6) - (grad/2));
                    g = (temp * 128) / grad;
                } else if (i < (max/3) - (grad/2)) {
                    // Second sixth: g=128
                    g = 128;
                } else if (i < (max/3) + (grad/2)) {
                    // Transition to third sixth (128 to 255)
                    temp = i - ((max/3) - (grad/2));
                    g = 128 + ((temp * 127) / grad);
                } else if (i < (max/2) - (grad/2)) {
                    // Third sixth: g=255
                    g = 255;
                } else if (i < (2*max/3) - (grad/2)) {
                    // Fourth sixth: g=255
                    g = 255;
                } else if (i < (2*max/3) + (grad/2)) {
                    // Transition to fifth sixth (255 to 0)
                    temp = i - ((2*max/3) - (grad/2));
                    g = 255 - ((temp * 255) / grad);
                } else {
                    // Fifth and sixth sixth: g=0
                    g = 0;
                }
                


               // blue
                if (i < (grad/2)) {
                    // Initial fade from 255 to 0
                    temp = (grad/2) - i;
                    b = (temp * 255) / grad;
                } else if (i < (2*max/3) - (grad/2)) {
                    // First two thirds: b=0
                    b = 0;
                } else if (i < (2*max/3) + (grad/2)) {
                    // Transition to last third (0 to 255)
                    temp = i - ((2*max/3) - (grad/2));
                    b = (temp * 255) / grad;
                } else {
                    // Last third: b=255
                    b = 255;
                }
                
    
                if (r > 255) r = 255;
                if (r < 0) r = 0;

                if (g > 255) g = 255;
                if (g < 0) g = 0;

                if (b > 255) b = 255;
                if (b < 0) b = 0;

                /*
                if (i < (max/6)-64) {
                    g = 0;
                } else if (i < (max/6)) {
                    temp = i-(max/6)-64; // 0-64
                    g = temp*4;
                } else if (i < (max - (max / 6) - 64)) {
                    g = 255;
                } else if (i < (max - (max / 6))) {
                    temp = i - (max - (max / 6)) - 64; // 0-64
                    g = 255 - (temp * 4);
                } else {
                    g = 0;
                }

                


                if (i < (max * 2/3) - 64) {
                    b = 0;
                } else if (i > (max * 2/3)) {
                    b = 255;
                } else {
                    temp = i - (max * 2/3) - 64; // 0-64
                    b = temp*4;
                }
                */

                //r = r / 2;
                //g = g / 2;
                //b = b / 2;

                strip_set_pixel_rgb(strip, i, r, g, b);
            }

            strip->buffer_dirty = true;
            strip_refresh(strip);
            vTaskDelay(100 / portTICK_PERIOD_MS);
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

void strip_rainbow_task(void *arg) {
    LOG("strip_rainbow_task()");
    strip_t *strip = (strip_t *)arg;

    int grad = 32;  // Transition length
    int temp = 0;
    int max = LED_LENGTH_1;
    int section = max / 6;  // Length of each color section

    // Define ROYGBV colors (Red, Orange, Yellow, Green, Blue, Violet)
    const uint8_t colors[7][3] = {
        {255, 0, 0},      // Red (wraps back to this)
        {255, 127, 0},    // Orange
        {255, 255, 0},    // Yellow
        {0, 255, 0},      // Green
        {0, 0, 255},      // Blue
        {148, 0, 211},    // Violet
        {255, 0, 0}       // Red (for final transition)
    };

    while (true) {
        for (int i = 0; i < max; i++) {
            int section_num = (i / section);
            int section_pos = i % section;
            
            // Handle the transition regions
            if (section_pos >= (section - grad/2) || section_pos < grad/2) {
                // Calculate position in transition
                int trans_pos;
                const uint8_t *color1;
                const uint8_t *color2;
                
                if (section_pos >= (section - grad/2)) {
                    // Transition to next color
                    trans_pos = section_pos - (section - grad/2);
                    color1 = colors[section_num];
                    color2 = colors[section_num + 1];
                } else {
                    // Transition from previous color
                    trans_pos = section_pos + grad/2;
                    color1 = colors[section_num];
                    color2 = colors[section_num];
                }

                // Calculate transitional colors
                uint8_t r = color1[0] + ((color2[0] - color1[0]) * trans_pos / grad);
                uint8_t g = color1[1] + ((color2[1] - color1[1]) * trans_pos / grad);
                uint8_t b = color1[2] + ((color2[2] - color1[2]) * trans_pos / grad);
                
                strip_set_pixel_rgb(strip, i, r, g, b);
            } else {
                // Solid color regions
                strip_set_pixel_rgb(strip, i, 
                    colors[section_num][0],
                    colors[section_num][1],
                    colors[section_num][2]);
            }
        }

        strip->buffer_dirty = true;
        strip_refresh(strip);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}