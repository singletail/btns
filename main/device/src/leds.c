#include "leds.h"
#include "grid.h"
#include "log.h"


// Color constants
//static const color_t RED = {255, 0, 0};
//static const color_t GREEN = {0, 255, 0};
// static const color_t BLUE = {0, 0, 255};
// static const color_t YELLOW = {255, 255, 0};
// static const color_t CYAN = {0, 255, 255};
// static const color_t MAGENTA = {255, 0, 255};
// static const color_t WHITE = {255, 255, 255};
// static const color_t BLACK = {0, 0, 0};

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

void cylon_task(void *arg) {
    INFO("cylon_task()");

    int cylon_position = 0;
    int cylon_speed = 1;
    int cylon_direction = 1;
    int cylon_length = 60;

    strip_t *strip = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    rmt_enable(strip->channel);
    strip_clear(strip);

    while (1) {
        strip_set_all_rgb(strip, 16, 0, 32);

        cylon_position += cylon_speed * cylon_direction;
        if (cylon_position >= STRIP_LENGTH_1 - cylon_length) {
            cylon_direction = -1;
        } else if (cylon_position <= 0) {
            cylon_direction = 1;
        }

        strip_set_range_rgb(strip, cylon_position, cylon_position + cylon_length, 255, 0, 0);
        strip_refresh(strip);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}



// V task
void v_task(void *arg) {
    // Create strips
    strip_t *strip = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
    strip_t *strip_2 = strip_create(1, STRIP_PIN_2, STRIP_LENGTH_2, false);
    strip_t *strip_3 = strip_create(2, STRIP_PIN_3, STRIP_LENGTH_3, false);
    rmt_enable(strip->channel);
    rmt_enable(strip_2->channel);
    rmt_enable(strip_3->channel);
    
   const int spacing = 6;  // Fixed spacing between bright LEDs
   int offset = 0;        // Position offset for movement

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