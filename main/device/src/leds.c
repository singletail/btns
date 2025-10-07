#include "leds.h"
#include "grid.h"
#include "log.h"
#include "btns.h"


void hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
    if (s == 0) {
        *r = *g = *b = v;
        return;
    }

    uint8_t region = h / 43;
    uint8_t remainder = (h - (region * 43)) * 6; 

    uint8_t p = (v * (255 - s)) >> 8;
    uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:  *r = v; *g = t; *b = p; break;
        case 1:  *r = q; *g = v; *b = p; break;
        case 2:  *r = p; *g = v; *b = t; break;
        case 3:  *r = p; *g = q; *b = v; break;
        case 4:  *r = t; *g = p; *b = v; break;
        default: *r = v; *g = p; *b = q; break;
    }
}

void hsv_to_rgb_obj(hsv_t *hsv, rgb_t *rgb) {
    if (hsv->s == 0) {
        rgb->r = rgb->g = rgb->b = hsv->v;
        return;
    }

    uint8_t region = hsv->h / 43;
    uint8_t remainder = (hsv->h - (region * 43)) * 6; 

    uint8_t p = (hsv->v * (255 - hsv->s)) >> 8;
    uint8_t q = (hsv->v * (255 - ((hsv->s * remainder) >> 8))) >> 8;
    uint8_t t = (hsv->v * (255 - ((hsv->s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:  rgb->r = hsv->v; rgb->g = t; rgb->b = p; break;
        case 1:  rgb->r = q; rgb->g = hsv->v; rgb->b = p; break;
        case 2:  rgb->r = p; rgb->g = hsv->v; rgb->b = t; break;
        case 3:  rgb->r = p; rgb->g = q; rgb->b = hsv->v; break;
        case 4:  rgb->r = t; rgb->g = p; rgb->b = hsv->v; break;
        default: rgb->r = hsv->v; rgb->g = p; rgb->b = q; break;
    }
}

void letter_test(void *arg) {
    INFO("letter_test()");

    strip_t *strip = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    rmt_enable(strip->channel);
    strip_clear(strip);


    strip_draw_char(strip, 3, 0, '2', 255, 226, 0);
    strip_draw_char(strip, 10, 0, '3', 255, 226, 0);
    strip_draw_char(strip, 18, 0, '3', 255, 226, 0);
    strip_draw_char(strip, 25, 0, '0', 255, 226, 0);

    strip_draw_char(strip, 3, 1, '2', 255, 226, 0);
    strip_draw_char(strip, 10, 1, '3', 255, 226, 0);
    strip_draw_char(strip, 18, 1, '3', 255, 226, 0);
    strip_draw_char(strip, 25, 1, '0', 255, 226, 0);

    strip_draw_char(strip, 4, 0, '2', 255, 226, 0);
    strip_draw_char(strip, 11, 0, '3', 255, 226, 0);
    strip_draw_char(strip, 17, 0, '3', 255, 226, 0);
    strip_draw_char(strip, 24, 0, '0', 255, 226, 0);

    strip_draw_char(strip, 4, 1, '2', 255, 226, 0);
    strip_draw_char(strip, 11, 1, '3', 255, 226, 0);
    strip_draw_char(strip, 17, 1, '3', 255, 226, 0);
    strip_draw_char(strip, 24, 1, '0', 255, 226, 0);

    strip_refresh(strip);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void grid_task(void *arg) {
    INFO("grid_task()");

    int grid_width = 32;
    int grid_height = 8;

    int dot_x = 0;
    int dot_y = 0;
    int dir_x = 1;
    int dir_y = 1;

    strip_t *strip = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    rmt_enable(strip->channel);
    strip_clear(strip);
    // strip_draw_line(strip, 10, 1, 26, 6, 0, 0, 255);
    // strip_refresh(strip);

    while (1) {
        strip_set_all_rgb(strip, 0, 0, 0);

        strip_draw_line(strip, 0, 0, 21, 7, 255, 0, 0);
        strip_draw_line(strip, 2, 0, 23, 7, 255, 128, 0);
        strip_draw_line(strip, 4, 0, 25, 7, 255, 255, 0);
        strip_draw_line(strip, 6, 0, 27, 7, 0, 255, 0);
        strip_draw_line(strip, 8, 0, 29, 7, 0, 0, 255);
        strip_draw_line(strip, 10, 0, 31, 7, 128, 0, 255);


        // bouncy dot
        dot_x += dir_x;
        dot_y += dir_y;

        if (dot_x >= (grid_width - 1)) {
            dir_x = -1;
        } else if (dot_x <= 0) {
            dir_x = 1;
        }

        if (dot_y >= (grid_height - 1)) {
            dir_y = -1;
        } else if (dot_y <= 0) {
            dir_y = 1;
        }

        strip_set_pixel_xy(strip, dot_x, dot_y, 255, 255, 255);
        strip_set_pixel_xy(strip, dot_x+1, dot_y, 255, 255, 255);
        strip_set_pixel_xy(strip, dot_x, dot_y+1, 255, 255, 255);
        strip_set_pixel_xy(strip, dot_x+1, dot_y+1, 255, 255, 255);

        strip_refresh(strip);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void hsv_task(void *arg) {
    INFO("hsv_task()");
    hsv_t hsv1 = {0, 255, 16};
    hsv_t hsv2 = {240, 255, 16};
    hsv_t hsv3 = {226, 255, 16};
    rgb_t rgb1 = {0, 0, 0};
    rgb_t rgb2 = {0, 0, 0};
    rgb_t rgb3 = {0, 0, 0};

    const int leds_len = STRIP_LENGTH_1;
    const int leds_spacing =16;
    int leds_offset = 0;

    const uint8_t leds_low = 8;
    const uint8_t leds_high = 128;

    int leds_pulse_level = 8;
    int leds_pulse_mod = 1;

    strip_t *strip1 = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    strip_t *strip2 = strip_create(1, STRIP_PIN_2, STRIP_LENGTH_2, false);
    strip_t *strip3 = strip_create(2, STRIP_PIN_3, STRIP_LENGTH_3, false);

    rmt_enable(strip1->channel);
    rmt_enable(strip2->channel);
    rmt_enable(strip3->channel);

    strip_clear(strip1);
    strip_clear(strip2);
    strip_clear(strip3);

    while (1) {
        hsv1.h++;
        hsv2.h++;
        hsv3.h++;

        if (hsv1.h >= 255) hsv1.h = hsv1.h - 255;
        if (hsv2.h >= 255) hsv2.h = hsv2.h - 255;
        if (hsv3.h >= 255) hsv3.h = hsv3.h - 255;

        leds_pulse_level += leds_pulse_mod;
        if (leds_pulse_level >= leds_high) {
            leds_pulse_mod = -1;
        } else if (leds_pulse_level <= leds_low) {
            leds_pulse_mod = 1;
        }

        for (int i = 0; i < leds_len; i++) {
            if ((i + leds_offset) % leds_spacing == 0) {
                hsv1.v = leds_high;
                hsv2.v = leds_high;
                hsv3.v = leds_high;
            } else if ((i + leds_offset) % leds_spacing == (leds_spacing - 1)) {
                hsv1.v = leds_high - 16;
                hsv2.v = leds_high - 16;
                hsv3.v = leds_high - 16;
            } else if ((i + leds_offset) % leds_spacing == (leds_spacing - 2)) {
                hsv1.v = leds_high - 32;
                hsv2.v = leds_high - 32;
                hsv3.v = leds_high - 32;
            } else if ((i + leds_offset) % leds_spacing == (leds_spacing - 3)) {
                hsv1.v = leds_high - 48;
                hsv2.v = leds_high - 48;
                hsv3.v = leds_high - 48;
            } else if ((i + leds_offset) % leds_spacing == (leds_spacing - 4)) {
                hsv1.v = leds_high - 64;
                hsv2.v = leds_high - 64;
                hsv3.v = leds_high - 64;
            } else {
                hsv1.v = leds_pulse_level;
                hsv2.v = leds_pulse_level;
                hsv3.v = leds_pulse_level;
            }
            hsv_to_rgb_obj(&hsv1, &rgb1);
            strip_set_pixel_rgb(strip1, i, rgb1.r, rgb1.g, rgb1.b);
            hsv_to_rgb_obj(&hsv2, &rgb2);
            strip_set_pixel_rgb(strip2, i, rgb2.r, rgb2.g, rgb2.b);
            hsv_to_rgb_obj(&hsv3, &rgb3);
            strip_set_pixel_rgb(strip3, i, rgb3.r, rgb3.g, rgb3.b);
        }

        strip_refresh(strip1);
        strip_refresh(strip2);
        strip_refresh(strip3);

        leds_offset = (leds_offset - 1);
        if (leds_offset < 0) {
            leds_offset = leds_spacing - 1;
        }

        vTaskDelay(pdMS_TO_TICKS(40));
    }
    
}

void cylon_task(void *arg) {
    INFO("cylon_task()");

    int cylon_mode = 0;
    int cylon_distance = 45;

    int cylon_pause_length = 60;
    int cylon_pause_counter = 0;

    int cylon_length = 12;
    int cylon_start = 0 - cylon_length;
    int cylon_end = cylon_distance;

    int cylon_position = 0;
    int cylon_direction = 1;

    strip_t *strip1 = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    strip_t *strip2 = strip_create(0, STRIP_PIN_2, STRIP_LENGTH_2, false);
    strip_t *strip3 = strip_create(0, STRIP_PIN_3, STRIP_LENGTH_3, false);

    rmt_enable(strip1->channel);
    rmt_enable(strip2->channel);
    rmt_enable(strip3->channel);

    strip_clear(strip1);
    strip_clear(strip2);
    strip_clear(strip3);

    while (1) {
        for (int i = 0; i < STRIP_LENGTH_1; i++) {
            if ( (i / 10) % 2 == 0) {
                strip_set_pixel_rgb(strip1, i, 24, 0, 0);
                strip_set_pixel_rgb(strip2, i, 24, 0, 0);
                strip_set_pixel_rgb(strip3, i, 24, 0, 0);
            } else {
                strip_set_pixel_rgb(strip1, i, 0, 0, 24);
                strip_set_pixel_rgb(strip2, i, 0, 0, 24);
                strip_set_pixel_rgb(strip3, i, 0, 0, 24);
            }
        }

        if (cylon_mode == 0) {
            cylon_position += cylon_direction;
            if (cylon_position >= cylon_end) {
                cylon_direction = -1;
            } else if (cylon_position <= cylon_start) {
                cylon_direction = 1;
                cylon_mode = 1;
            }

            //strip_set_range_rgb(strip, cylon_position - cylon_length, cylon_position, 128, 0, 0);
        
                for (int i = cylon_position; i < (cylon_position + cylon_length); i++) {
                    strip_set_pixel_rgb(strip1, i, 255, 0, 0);
                    strip_set_pixel_rgb(strip2, i, 255, 0, 0);
                    strip_set_pixel_rgb(strip3, i, 255, 0, 0);
            }
        
        } else {
            cylon_pause_counter++;
            if (cylon_pause_counter >= cylon_pause_length) {
                cylon_mode = 0;
                cylon_pause_counter = 0;
            }
        }

        strip_refresh(strip1);
        strip_refresh(strip2);
        strip_refresh(strip3);

        vTaskDelay(pdMS_TO_TICKS(15));

    }
}



// V task
void v_task(void *arg) {
    
    strip_t *strip = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    strip_t *strip_2 = strip_create(1, STRIP_PIN_2, STRIP_LENGTH_2, false);
    strip_t *strip_3 = strip_create(2, STRIP_PIN_3, STRIP_LENGTH_3, false);

    rmt_enable(strip->channel);
    rmt_enable(strip_2->channel);
    rmt_enable(strip_3->channel);
    
   const int spacing = 6;
   int offset = 0;

    while (1) {
        strip_set_all_rgb(strip, 32, 0, 0);
        strip_set_all_rgb(strip_2, 32, 0, 32);
        strip_set_all_rgb(strip_3, 0, 0, 32);

        for (int i = 0; i < 46; i++) {
                if ((i + offset) % spacing == 0) {
                    strip_set_pixel_rgb(strip, i, 255, 0, 0);
                    strip_set_pixel_rgb(strip_2, i, 255, 0, 255);
                    strip_set_pixel_rgb(strip_3, i, 0, 0, 255);
            }
        }

        strip_refresh(strip);
        strip_refresh(strip_2);
        strip_refresh(strip_3);

        offset = (offset - 1);
        if (offset < 0) {
            offset = spacing - 1;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void pulse_task(void *arg) {
    INFO("pulse_task()");
    rgb_t pulse = {0, 0, 0};

    uint8_t pulse_mode = 0;
    uint32_t pulse_counter = 0;
    uint32_t pulse_pause = 150;

    strip_t *strip_1 = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    strip_t *strip_2 = strip_create(1, STRIP_PIN_2, STRIP_LENGTH_2, false);
    strip_t *strip_3 = strip_create(2, STRIP_PIN_3, STRIP_LENGTH_3, false);

    rmt_enable(strip_1->channel);
    rmt_enable(strip_2->channel);
    rmt_enable(strip_3->channel);

    strip_clear(strip_1);
    strip_clear(strip_2);
    strip_clear(strip_3);

    strip_refresh(strip_1);
    strip_refresh(strip_2);
    strip_refresh(strip_3);

    while (1) {

        if (pulse_mode == 0) {
            pulse.b += 1;
            if (pulse.b >= 128) {
                pulse_mode = 1;
            }
        } else if (pulse_mode == 1) {
            pulse.b -= 1;
            if (pulse.b <= 0) {
                pulse_counter = 0;
                pulse_mode = 2;
            }
        } else if (pulse_mode == 2) {
            pulse_counter++;
            if (pulse_counter >= pulse_pause) {
                pulse_mode = 0;
            }
        }

        strip_set_all_rgb(strip_1, pulse.r, pulse.g, pulse.b);
        strip_set_all_rgb(strip_2, pulse.r, pulse.g, pulse.b);
        strip_set_all_rgb(strip_3, pulse.r, pulse.g, pulse.b);

        strip_refresh(strip_1);
        strip_refresh(strip_2);
        strip_refresh(strip_3);

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

void police_task(void *arg) {
    INFO("police_task()");

    // Create and enable strips
    strip_t *strip_1 = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    strip_t *strip_2 = strip_create(1, STRIP_PIN_2, STRIP_LENGTH_2, false);
    strip_t *strip_3 = strip_create(2, STRIP_PIN_3, STRIP_LENGTH_3, false);

    rmt_enable(strip_1->channel);
    rmt_enable(strip_2->channel);
    rmt_enable(strip_3->channel);

    strip_clear(strip_1);
    strip_clear(strip_2);
    strip_clear(strip_3);

    const uint8_t RED = 255;
    const uint8_t BLUE = 255;
    const uint8_t WHITE = 255;
    const uint8_t AMBIENT = 10;

    const int SECTION_SIZE = 8;
    const int NUM_SECTIONS = STRIP_LENGTH_1 / SECTION_SIZE;
    const int STROBE_LENGTH = 25;  // 50;  // duration per strobe
    const int BURST_COUNT = 6; // 4;
    const int BURST_DELAY = 100; // 400;  // time before switching sides

    bool red_side_active = true;

    while (1) {
        for (int burst = 0; burst < BURST_COUNT; burst++) {
            // Set background ambient glow
            strip_set_all_rgb(strip_1, AMBIENT, 0, AMBIENT);
            strip_set_all_rgb(strip_2, AMBIENT, 0, AMBIENT);
            strip_set_all_rgb(strip_3, AMBIENT, 0, AMBIENT);

            // Calculate center "wig-wag" position (optional)
            int center_start = (STRIP_LENGTH_1 / 2) - (SECTION_SIZE / 2);
            int center_end = center_start + SECTION_SIZE;

            for (int i = 0; i < NUM_SECTIONS; i++) {
                int start = i * SECTION_SIZE;
                int end = start + SECTION_SIZE;
                bool is_left = (i < NUM_SECTIONS / 2);

                if (red_side_active && is_left) {
                    // Red side strobe
                    strip_set_range_rgb(strip_1, start, end, RED, 0, 0);
                    strip_set_range_rgb(strip_2, start, end, RED, 0, 0);
                    strip_set_range_rgb(strip_3, start, end, RED, 0, 0);
                } else if (!red_side_active && !is_left) {
                    // Blue side strobe
                    strip_set_range_rgb(strip_1, start, end, 0, 0, BLUE);
                    strip_set_range_rgb(strip_2, start, end, 0, 0, BLUE);
                    strip_set_range_rgb(strip_3, start, end, 0, 0, BLUE);
                }
            }

            // White strobe on center every other flash
            if (burst % 2 == 0) {
                strip_set_range_rgb(strip_1, center_start, center_end, WHITE, WHITE, WHITE);
                strip_set_range_rgb(strip_2, center_start, center_end, WHITE, WHITE, WHITE);
                strip_set_range_rgb(strip_3, center_start, center_end, WHITE, WHITE, WHITE);
            }

            strip_refresh(strip_1);
            strip_refresh(strip_2);
            strip_refresh(strip_3);

            vTaskDelay(pdMS_TO_TICKS(STROBE_LENGTH));
            strip_clear(strip_1);
            strip_clear(strip_2);
            strip_clear(strip_3);
            vTaskDelay(pdMS_TO_TICKS(STROBE_LENGTH));
        }

        // Wait briefly before switching sides
        vTaskDelay(pdMS_TO_TICKS(BURST_DELAY));
        red_side_active = !red_side_active;
    }
}

// Animation modes
typedef enum {
    MODE_PULSE_BLUE,
    MODE_PULSE_RED,
    MODE_HSV,
    MODE_V,
    MODE_CYLON,
    MODE_POLICE,
    MODE_COUNT   // Used to determine number of modes
} led_mode_t;

// Global variable to track current animation mode
static volatile led_mode_t current_mode = MODE_PULSE_BLUE;

seg7_t *s7;

// Button handler for changing LED modes
void led_mode_btn_handler(void *arg, btn_event_t event) {
    if (event == BTN_EVENT_CLICKED) {
        // Cycle to next mode
        current_mode = (current_mode + 1) % MODE_COUNT;
        INFO("Switched to LED mode %d", current_mode);

        seg7_clear(s7);
        seg7_display_number(s7, current_mode);
    }
}

void multi_mode_led_task(void *arg) {
    INFO("multi_mode_led_task()");
#ifdef PIN_SEG7_CLK
    s7 = seg7_new(PIN_SEG7_CLK, PIN_SEG7_DIO);
    seg7_clear(s7);
    seg7_display_number(s7, current_mode);
#endif
    
    // Create strips only once
    strip_t *strip_1 = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    strip_t *strip_2 = strip_create(1, STRIP_PIN_2, STRIP_LENGTH_2, false);
    strip_t *strip_3 = strip_create(2, STRIP_PIN_3, STRIP_LENGTH_3, false);

    rmt_enable(strip_1->channel);
    rmt_enable(strip_2->channel);
    rmt_enable(strip_3->channel);

    strip_clear(strip_1);
    strip_clear(strip_2);
    strip_clear(strip_3);
    
    // Animation state variables
    
    // Pulse mode variables
    rgb_t pulse = {0, 0, 0};
    uint8_t pulse_mode = 0;
    uint32_t pulse_counter = 0;
    uint32_t pulse_pause = 150;
    
    // V mode variables
    int v_spacing = 6;
    int v_offset = 0;
    
    // Cylon mode variables
    int cylon_position = 0;
    int cylon_direction = 1;
    int cylon_length = 12;
    int cylon_distance = 45;
    
    // HSV mode variables
    hsv_t hsv1 = {0, 255, 16};
    hsv_t hsv2 = {240, 255, 16};
    hsv_t hsv3 = {226, 255, 16};
    rgb_t rgb1 = {0, 0, 0};
    rgb_t rgb2 = {0, 0, 0};
    rgb_t rgb3 = {0, 0, 0};
    int hsv_offset = 0;
    int hsv_spacing = 16;
    int hsv_pulse_level = 8;
    int hsv_pulse_mod = 1;
    const uint8_t hsv_low = 8;
    const uint8_t hsv_high = 128;
    
    // Police mode variables
    const uint8_t police_red = 255;
    const uint8_t police_blue = 255;
    const uint8_t police_white = 255;
    const uint8_t police_ambient = 10;
    const int police_section_size = 8;
    const int police_sections = STRIP_LENGTH_1 / police_section_size;
    const int police_strobe_length = 25;
    const int police_burst_count = 6;
    const int police_burst_delay = 100;
    bool police_red_active = true;
    int police_burst = 0;
    
    // Main animation loop
    while (1) {
        // Handle current animation mode
        switch (current_mode) {
            case MODE_PULSE_BLUE:
                // Blue pulse (same as pulse_task)
                if (pulse_mode == 0) {
                    pulse.b += 1;
                    if (pulse.b >= 128) pulse_mode = 1;
                } else if (pulse_mode == 1) {
                    pulse.b -= 1;
                    if (pulse.b <= 0) {
                        pulse_counter = 0;
                        pulse_mode = 2;
                    }
                } else if (pulse_mode == 2) {
                    pulse_counter++;
                    if (pulse_counter >= pulse_pause) pulse_mode = 0;
                }
                
                strip_set_all_rgb(strip_1, pulse.r, pulse.g, pulse.b);
                strip_set_all_rgb(strip_2, pulse.r, pulse.g, pulse.b);
                strip_set_all_rgb(strip_3, pulse.r, pulse.g, pulse.b);
                
                vTaskDelay(pdMS_TO_TICKS(15));
                break;
                
            case MODE_PULSE_RED:
                // Red pulse (modified pulse_task)
                if (pulse_mode == 0) {
                    pulse.r += 1;
                    if (pulse.r >= 128) pulse_mode = 1;
                } else if (pulse_mode == 1) {
                    pulse.r -= 1;
                    if (pulse.r <= 0) {
                        pulse_counter = 0;
                        pulse_mode = 2;
                    }
                } else if (pulse_mode == 2) {
                    pulse_counter++;
                    if (pulse_counter >= pulse_pause) pulse_mode = 0;
                }
                
                strip_set_all_rgb(strip_1, pulse.r, pulse.g, pulse.b);
                strip_set_all_rgb(strip_2, pulse.r, pulse.g, pulse.b);
                strip_set_all_rgb(strip_3, pulse.r, pulse.g, pulse.b);
                
                vTaskDelay(pdMS_TO_TICKS(15));
                break;
                
            case MODE_HSV:
                // HSV pattern
                hsv1.h++;
                hsv2.h++;
                hsv3.h++;

                if (hsv1.h >= 255) hsv1.h = hsv1.h - 255;
                if (hsv2.h >= 255) hsv2.h = hsv2.h - 255;
                if (hsv3.h >= 255) hsv3.h = hsv3.h - 255;

                hsv_pulse_level += hsv_pulse_mod;
                if (hsv_pulse_level >= hsv_high) {
                    hsv_pulse_mod = -1;
                } else if (hsv_pulse_level <= hsv_low) {
                    hsv_pulse_mod = 1;
                }

                for (int i = 0; i < STRIP_LENGTH_1; i++) {
                    if ((i + hsv_offset) % hsv_spacing == 0) {
                        hsv1.v = hsv_high;
                        hsv2.v = hsv_high;
                        hsv3.v = hsv_high;
                    } else if ((i + hsv_offset) % hsv_spacing == (hsv_spacing - 1)) {
                        hsv1.v = hsv_high - 16;
                        hsv2.v = hsv_high - 16;
                        hsv3.v = hsv_high - 16;
                    } else {
                        hsv1.v = hsv_pulse_level;
                        hsv2.v = hsv_pulse_level;
                        hsv3.v = hsv_pulse_level;
                    }
                    
                    hsv_to_rgb_obj(&hsv1, &rgb1);
                    hsv_to_rgb_obj(&hsv2, &rgb2);
                    hsv_to_rgb_obj(&hsv3, &rgb3);
                    
                    strip_set_pixel_rgb(strip_1, i, rgb1.r, rgb1.g, rgb1.b);
                    strip_set_pixel_rgb(strip_2, i, rgb2.r, rgb2.g, rgb2.b);
                    strip_set_pixel_rgb(strip_3, i, rgb3.r, rgb3.g, rgb3.b);
                }

                hsv_offset = (hsv_offset - 1);
                if (hsv_offset < 0) {
                    hsv_offset = hsv_spacing - 1;
                }
                
                vTaskDelay(pdMS_TO_TICKS(40));
                break;
                
            case MODE_V:
                // V pattern
                strip_set_all_rgb(strip_1, 32, 0, 0);
                strip_set_all_rgb(strip_2, 32, 0, 32);
                strip_set_all_rgb(strip_3, 0, 0, 32);

                for (int i = 0; i < 46; i++) {
                    if ((i + v_offset) % v_spacing == 0) {
                        strip_set_pixel_rgb(strip_1, i, 255, 0, 0);
                        strip_set_pixel_rgb(strip_2, i, 255, 0, 255);
                        strip_set_pixel_rgb(strip_3, i, 0, 0, 255);
                    }
                }

                v_offset = (v_offset - 1);
                if (v_offset < 0) {
                    v_offset = v_spacing - 1;
                }
                
                vTaskDelay(pdMS_TO_TICKS(50));
                break;
                
            case MODE_CYLON:
                // Cylon scanner pattern
                for (int i = 0; i < STRIP_LENGTH_1; i++) {
                    if ((i / 10) % 2 == 0) {
                        strip_set_pixel_rgb(strip_1, i, 24, 0, 0);
                        strip_set_pixel_rgb(strip_2, i, 24, 0, 0);
                        strip_set_pixel_rgb(strip_3, i, 24, 0, 0);
                    } else {
                        strip_set_pixel_rgb(strip_1, i, 0, 0, 24);
                        strip_set_pixel_rgb(strip_2, i, 0, 0, 24);
                        strip_set_pixel_rgb(strip_3, i, 0, 0, 24);
                    }
                }
                
                cylon_position += cylon_direction;
                if (cylon_position >= cylon_distance) {
                    cylon_direction = -1;
                } else if (cylon_position <= 0) {
                    cylon_direction = 1;
                }
                
                for (int i = cylon_position; i < (cylon_position + cylon_length); i++) {
                    strip_set_pixel_rgb(strip_1, i, 255, 0, 0);
                    strip_set_pixel_rgb(strip_2, i, 255, 0, 0);
                    strip_set_pixel_rgb(strip_3, i, 255, 0, 0);
                }
                
                vTaskDelay(pdMS_TO_TICKS(15));
                break;
                
            case MODE_POLICE:
                // Police strobe pattern
                if (police_burst < police_burst_count) {
                    // Set background ambient glow
                    strip_set_all_rgb(strip_1, police_ambient, 0, police_ambient);
                    strip_set_all_rgb(strip_2, police_ambient, 0, police_ambient);
                    strip_set_all_rgb(strip_3, police_ambient, 0, police_ambient);

                    // Calculate center position
                    int center_start = (STRIP_LENGTH_1 / 2) - (police_section_size / 2);
                    int center_end = center_start + police_section_size;

                    for (int i = 0; i < police_sections; i++) {
                        int start = i * police_section_size;
                        int end = start + police_section_size;
                        bool is_left = (i < police_sections / 2);

                        if (police_red_active && is_left) {
                            // Red side strobe
                            strip_set_range_rgb(strip_1, start, end, police_red, 0, 0);
                            strip_set_range_rgb(strip_2, start, end, police_red, 0, 0);
                            strip_set_range_rgb(strip_3, start, end, police_red, 0, 0);
                        } else if (!police_red_active && !is_left) {
                            // Blue side strobe
                            strip_set_range_rgb(strip_1, start, end, 0, 0, police_blue);
                            strip_set_range_rgb(strip_2, start, end, 0, 0, police_blue);
                            strip_set_range_rgb(strip_3, start, end, 0, 0, police_blue);
                        }
                    }

                    // White strobe on center every other flash
                    if (police_burst % 2 == 0) {
                        strip_set_range_rgb(strip_1, center_start, center_end, police_white, police_white, police_white);
                        strip_set_range_rgb(strip_2, center_start, center_end, police_white, police_white, police_white);
                        strip_set_range_rgb(strip_3, center_start, center_end, police_white, police_white, police_white);
                    }

                    strip_refresh(strip_1);
                    strip_refresh(strip_2);
                    strip_refresh(strip_3);

                    vTaskDelay(pdMS_TO_TICKS(police_strobe_length));
                    strip_clear(strip_1);
                    strip_clear(strip_2);
                    strip_clear(strip_3);
                    strip_refresh(strip_1);
                    strip_refresh(strip_2);
                    strip_refresh(strip_3);
                    vTaskDelay(pdMS_TO_TICKS(police_strobe_length));
                    
                    police_burst++;
                } else {
                    // Reset for next cycle
                    police_burst = 0;
                    police_red_active = !police_red_active;
                    vTaskDelay(pdMS_TO_TICKS(police_burst_delay));
                }
                break;
                
            default:
                vTaskDelay(pdMS_TO_TICKS(50));
                break;
        }
        
        // Always refresh strips at the end of each animation loop
        strip_refresh(strip_1);
        strip_refresh(strip_2);
        strip_refresh(strip_3);
    }
}

/*
// Demo types
typedef enum {
    DEMO_SCROLL_TEXT,
    DEMO_BOUNCE,
    DEMO_RAINBOW,
    DEMO_COUNT
} demo_type_t;

// Grid demo task
void grid_demo_task(void *arg) {
    ESP_LOGI(TAG, "grid_demo_task()");
    
    // Create a strip
    strip_t *strip = strip_create(0, STRIP_PIN_1, GRID_TOTAL_PIXELS, false);
    if (!strip) {
        ESP_LOGE(TAG, "Failed to create strip");
        vTaskDelete(NULL);
        return;
    }
    
    // Create a grid
    grid_t *grid = grid_create(strip, GRID_WIDTH, GRID_HEIGHT, true);
    if (!grid) {
        ESP_LOGE(TAG, "Failed to create grid");
        vTaskDelete(NULL);
        return;
    }
    
    // Demo state
    demo_type_t demo_type = DEMO_SCROLL_TEXT;
    uint32_t demo_counter = 0;
    
    // Main loop
    while (1) {
        // Clear the grid
        grid_clear(grid);
        
        // Run the current demo
        switch (demo_type) {
            case DEMO_SCROLL_TEXT:
                // Scroll text demo
                grid_scroll_text(grid, "Hello World!", NULL, WHITE, 1, 100);
                break;
                
            case DEMO_BOUNCE:
                // Bounce demo
                {
                    int x = (demo_counter / 10) % (GRID_WIDTH - 4);
                    int y = (demo_counter / 5) % (GRID_HEIGHT - 2);
                    grid_fill_rect(grid, x, y, 4, 2, RED);
                }
                break;
                
            case DEMO_RAINBOW:
                // Rainbow demo
                {
                    for (int i = 0; i < GRID_WIDTH; i++) {
                        color_t color;
                        color.r = (uint8_t)((sin(i * 0.1 + demo_counter * 0.05) + 1) * 127);
                        color.g = (uint8_t)((sin(i * 0.1 + demo_counter * 0.05 + 2) + 1) * 127);
                        color.b = (uint8_t)((sin(i * 0.1 + demo_counter * 0.05 + 4) + 1) * 127);
                        grid_draw_line(grid, i, 0, i, GRID_HEIGHT - 1, color);
                    }
                }
                break;
                
            default:
                break;
        }
        
        // Refresh the grid
        grid_refresh(grid);
        
        // Increment demo counter
        demo_counter++;
        
        // Switch demo type every 10 seconds
        if (demo_counter % 100 == 0) {
            demo_type = (demo_type + 1) % DEMO_COUNT;
        }
        
        // Wait a bit
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

*/

void maybe_task(void *arg) {
    INFO("maybe_task()");

    // Define our colors
    rgb_t rgb1 = {0, 120, 128};       // cyan
    rgb_t rgb2 = {64, 64, 64};   // whiteish
    rgb_t rgb3 = {118, 0, 128};   // Magenta

    // Current transition colors
    rgb_t start_color = rgb1;
    rgb_t end_color = rgb2;
    
    // Animation parameters
    int gradient_size = 24;
    int gradient_position = -gradient_size;
    int state = 0;  // 0: transitioning, 1: pausing
    int current_transition = 0;  // 0: rgb1->rgb2, 1: rgb2->rgb3, 2: rgb3->rgb1
    int pause_counter = 0;
    const int pause_duration = 50;  // 2 seconds @ 40ms per tick

    strip_t *strip1 = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    strip_t *strip2 = strip_create(0, STRIP_PIN_2, STRIP_LENGTH_2, false);
    strip_t *strip3 = strip_create(0, STRIP_PIN_3, STRIP_LENGTH_3, false);
    rmt_enable(strip1->channel);
    rmt_enable(strip2->channel);
    rmt_enable(strip3->channel);
    strip_clear(strip1);
    strip_clear(strip2);
    strip_clear(strip3);

    while (1) {
        if (state == 0) {  // Transitioning
            // Fill the strip based on gradient position
            for (int i = 0; i < STRIP_LENGTH_1; i++) {
                if (i < gradient_position) {
                    // Area that has passed the gradient - full end_color
                    strip_set_pixel_rgb(strip1, i, end_color.r, end_color.g, end_color.b);
                } else if (i < gradient_position + gradient_size) {
                    // Inside the gradient - calculate transition color
                    int pos = gradient_size - (i - gradient_position) - 1;
                    uint8_t r = start_color.r + ((end_color.r - start_color.r) * pos) / gradient_size;
                    uint8_t g = start_color.g + ((end_color.g - start_color.g) * pos) / gradient_size;
                    uint8_t b = start_color.b + ((end_color.b - start_color.b) * pos) / gradient_size;
                    strip_set_pixel_rgb(strip1, i, r, g, b);
                } else {
                    // Not yet reached by gradient - still start_color
                    strip_set_pixel_rgb(strip1, i, start_color.r, start_color.g, start_color.b);
                }
            }
            
            // Advance gradient position
            gradient_position++;
            
            // If gradient has passed through
            if (gradient_position > STRIP_LENGTH_1) {
                state = 1;  // Switch to pause state
                pause_counter = 0;
            }
        } else {  // Pausing
            // Fill entire strip with end_color
            for (int i = 0; i < STRIP_LENGTH_1; i++) {
                strip_set_pixel_rgb(strip1, i, end_color.r, end_color.g, end_color.b);
                strip_set_pixel_rgb(strip2, i, end_color.r, end_color.g, end_color.b);
                strip_set_pixel_rgb(strip3, i, end_color.r, end_color.g, end_color.b);
            }
            
            // Count pause duration
            pause_counter++;
            if (pause_counter >= pause_duration) {
                // Move to next transition
                current_transition = (current_transition + 1) % 3;
                
                // Set colors for next transition
                if (current_transition == 0) {
                    start_color = rgb1;
                    end_color = rgb2;
                } else if (current_transition == 1) {
                    start_color = rgb2;
                    end_color = rgb3;
                } else {  // current_transition == 2
                    start_color = rgb3;
                    end_color = rgb1;
                }
                
                // Reset for next transition
                state = 0;
                gradient_position = -gradient_size;
            }
        }

        // Update the strip
        strip_refresh(strip1);
        strip_refresh(strip2);
        strip_refresh(strip3);
        
        // Control the animation speed
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void quarterflash_task(void *arg) {
    INFO("quarterflash_task()");
    
    strip_t *strip = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    rmt_enable(strip->channel);
    strip_clear(strip);
    
    // For 8x32 grid = 256 total pixels
    const int total_pixels = STRIP_LENGTH_1;
    const int quarter_pixels = total_pixels / 4;  // 64 pixels
    
    // Array to track which pixels are selected
    bool selected_pixels[STRIP_LENGTH_1] = {false};
    
    while (1) {
        // Clear the strip
        strip_clear(strip);
        
        // Reset selection array
        for (int i = 0; i < total_pixels; i++) {
            selected_pixels[i] = false;
        }
        
        // Randomly select quarter of pixels with even distribution
        int selected_count = 0;
        while (selected_count < quarter_pixels) {
            int random_pixel = esp_random() % total_pixels;
            if (!selected_pixels[random_pixel]) {
                selected_pixels[random_pixel] = true;
                selected_count++;
            }
        }
        
        // Set selected pixels to random red or amber
        for (int i = 0; i < total_pixels; i++) {
            if (selected_pixels[i]) {
                // Random choice between red and amber
                if (esp_random() % 2) {
                    strip_set_pixel_rgb(strip, i, 255, 0, 0);      // Red
                } else {
                    strip_set_pixel_rgb(strip, i, 255, 128, 0);    // Amber
                }
            }
            // All other pixels remain dark (already cleared)
        }
        
        // Refresh the strip
        strip_refresh(strip);
        
        // Wait 2 seconds
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}