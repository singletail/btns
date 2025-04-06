#include "leds.h"

/*
// Define TAG for logging
static const char *TAG = "leds";

// Color constants
static const color_t RED = {255, 0, 0};
static const color_t GREEN = {0, 255, 0};
static const color_t BLUE = {0, 0, 255};
static const color_t YELLOW = {255, 255, 0};
static const color_t CYAN = {0, 255, 255};
static const color_t MAGENTA = {255, 0, 255};
static const color_t WHITE = {255, 255, 255};
// static const color_t BLACK = {0, 0, 0};

// Grid task

void grid_task(void *arg) {
    ESP_LOGI(TAG, "grid_task()");
    
    // Create a strip
    strip_t *strip = strip_create(0, STRIP_PIN_1, STRIP_LENGTH_1, false);
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
    
    // Main loop
    while (1) {
        // Clear the grid
        grid_clear(grid);
        
        // Draw a rectangle
        grid_draw_rect(grid, 10, 2, 44, 4, RED);
        
        // Draw a circle
        grid_draw_circle(grid, 32, 4, 3, GREEN);
        
        // Draw some text
        grid_draw_string(grid, 12, 3, "Hello", NULL, BLUE);
        
        // Refresh the grid
        grid_refresh(grid);
        
        // Wait a bit
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

*/

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
        strip_set_all_rgb(strip, 64, 0, 0);
        strip_set_all_rgb(strip_2, 64, 0, 64);
        strip_set_all_rgb(strip_3, 0, 0, 64);

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