/*

#include "grid.h"
#include "strip.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <math.h>

// Define TAG for logging
static const char *TAG = "grid";

// Global grid object
static grid_t *g_grid = NULL;

// Simple 5x7 font data (ASCII 32-127)
// Each character is 5 bits wide by 7 bits high
// Bitmap data is stored in a compact format
static const uint8_t font_5x7[] = {
    // Space (32)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // ! (33)
    0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08,
    // " (34)
    0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00,
    // # (35)
    0x14, 0x14, 0x3E, 0x14, 0x3E, 0x14, 0x14,
    // $ (36)
    0x08, 0x3E, 0x28, 0x3E, 0x0A, 0x3E, 0x08,
    // % (37)
    0x30, 0x32, 0x04, 0x08, 0x10, 0x26, 0x06,
    // & (38)
    0x18, 0x24, 0x28, 0x10, 0x2A, 0x24, 0x1A,
    // ' (39)
    0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
    // ( (40)
    0x04, 0x08, 0x10, 0x10, 0x10, 0x08, 0x04,
    // ) (41)
    0x10, 0x08, 0x04, 0x04, 0x04, 0x08, 0x10,
    // * (42)
    0x00, 0x08, 0x2A, 0x1C, 0x2A, 0x08, 0x00,
    // + (43)
    0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00,
    // , (44)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x10,
    // - (45)
    0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00,
    // . (46)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
    // / (47)
    0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    // 0 (48)
    0x1C, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1C,
    // 1 (49)
    0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x3E,
    // 2 (50)
    0x1C, 0x22, 0x02, 0x0C, 0x10, 0x20, 0x3E,
    // 3 (51)
    0x3E, 0x02, 0x04, 0x0C, 0x02, 0x22, 0x1C,
    // 4 (52)
    0x04, 0x0C, 0x14, 0x24, 0x3E, 0x04, 0x04,
    // 5 (53)
    0x3E, 0x20, 0x3C, 0x02, 0x02, 0x22, 0x1C,
    // 6 (54)
    0x0C, 0x10, 0x20, 0x3C, 0x22, 0x22, 0x1C,
    // 7 (55)
    0x3E, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20,
    // 8 (56)
    0x1C, 0x22, 0x22, 0x1C, 0x22, 0x22, 0x1C,
    // 9 (57)
    0x1C, 0x22, 0x22, 0x1E, 0x02, 0x04, 0x18,
    // : (58)
    0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00,
    // ; (59)
    0x00, 0x00, 0x08, 0x00, 0x00, 0x08, 0x10,
    // < (60)
    0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02,
    // = (61)
    0x00, 0x00, 0x3E, 0x00, 0x3E, 0x00, 0x00,
    // > (62)
    0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10,
    // ? (63)
    0x1C, 0x22, 0x02, 0x04, 0x08, 0x00, 0x08,
    // @ (64)
    0x1C, 0x22, 0x02, 0x1A, 0x2A, 0x2A, 0x1C,
    // A (65)
    0x08, 0x14, 0x22, 0x22, 0x3E, 0x22, 0x22,
    // B (66)
    0x3C, 0x22, 0x22, 0x3C, 0x22, 0x22, 0x3C,
    // C (67)
    0x1C, 0x22, 0x20, 0x20, 0x20, 0x22, 0x1C,
    // D (68)
    0x3C, 0x22, 0x22, 0x22, 0x22, 0x22, 0x3C,
    // E (69)
    0x3E, 0x20, 0x20, 0x3C, 0x20, 0x20, 0x3E,
    // F (70)
    0x3E, 0x20, 0x20, 0x3C, 0x20, 0x20, 0x20,
    // G (71)
    0x1C, 0x22, 0x20, 0x20, 0x26, 0x22, 0x1C,
    // H (72)
    0x22, 0x22, 0x22, 0x3E, 0x22, 0x22, 0x22,
    // I (73)
    0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E,
    // J (74)
    0x02, 0x02, 0x02, 0x02, 0x22, 0x22, 0x1C,
    // K (75)
    0x22, 0x24, 0x28, 0x30, 0x28, 0x24, 0x22,
    // L (76)
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3E,
    // M (77)
    0x22, 0x36, 0x2A, 0x2A, 0x22, 0x22, 0x22,
    // N (78)
    0x22, 0x22, 0x32, 0x2A, 0x26, 0x22, 0x22,
    // O (79)
    0x1C, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1C,
    // P (80)
    0x3C, 0x22, 0x22, 0x3C, 0x20, 0x20, 0x20,
    // Q (81)
    0x1C, 0x22, 0x22, 0x22, 0x2A, 0x24, 0x1A,
    // R (82)
    0x3C, 0x22, 0x22, 0x3C, 0x28, 0x24, 0x22,
    // S (83)
    0x1C, 0x22, 0x20, 0x1C, 0x02, 0x22, 0x1C,
    // T (84)
    0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    // U (85)
    0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1C,
    // V (86)
    0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x08,
    // W (87)
    0x22, 0x22, 0x22, 0x2A, 0x2A, 0x36, 0x22,
    // X (88)
    0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22,
    // Y (89)
    0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08,
    // Z (90)
    0x3E, 0x02, 0x04, 0x08, 0x10, 0x20, 0x3E,
    // [ (91)
    0x3E, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3E,
    // \ (92)
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02,
    // ] (93)
    0x3E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3E,
    // ^ (94)
    0x08, 0x14, 0x22, 0x00, 0x00, 0x00, 0x00,
    // _ (95)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E,
    // ` (96)
    0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00,
    // a (97)
    0x00, 0x00, 0x1C, 0x02, 0x1E, 0x22, 0x1E,
    // b (98)
    0x20, 0x20, 0x3C, 0x22, 0x22, 0x22, 0x3C,
    // c (99)
    0x00, 0x00, 0x1C, 0x22, 0x20, 0x22, 0x1C,
    // d (100)
    0x02, 0x02, 0x1E, 0x22, 0x22, 0x22, 0x1E,
    // e (101)
    0x00, 0x00, 0x1C, 0x22, 0x3E, 0x20, 0x1C,
    // f (102)
    0x0C, 0x12, 0x10, 0x38, 0x10, 0x10, 0x10,
    // g (103)
    0x00, 0x00, 0x1E, 0x22, 0x22, 0x1E, 0x02, 0x1C,
    // h (104)
    0x20, 0x20, 0x3C, 0x22, 0x22, 0x22, 0x22,
    // i (105)
    0x08, 0x00, 0x18, 0x08, 0x08, 0x08, 0x1C,
    // j (106)
    0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x24, 0x18,
    // k (107)
    0x20, 0x20, 0x22, 0x24, 0x38, 0x24, 0x22,
    // l (108)
    0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C,
    // m (109)
    0x00, 0x00, 0x36, 0x2A, 0x2A, 0x2A, 0x22,
    // n (110)
    0x00, 0x00, 0x3C, 0x22, 0x22, 0x22, 0x22,
    // o (111)
    0x00, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C,
    // p (112)
    0x00, 0x00, 0x3C, 0x22, 0x22, 0x3C, 0x20, 0x20,
    // q (113)
    0x00, 0x00, 0x1E, 0x22, 0x22, 0x1E, 0x02, 0x02,
    // r (114)
    0x00, 0x00, 0x3C, 0x22, 0x20, 0x20, 0x20,
    // s (115)
    0x00, 0x00, 0x1E, 0x20, 0x1C, 0x02, 0x3C,
    // t (116)
    0x10, 0x10, 0x38, 0x10, 0x10, 0x12, 0x0C,
    // u (117)
    0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x1E,
    // v (118)
    0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x08,
    // w (119)
    0x00, 0x00, 0x22, 0x22, 0x2A, 0x2A, 0x14,
    // x (120)
    0x00, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22,
    // y (121)
    0x00, 0x00, 0x22, 0x22, 0x22, 0x1E, 0x02, 0x1C,
    // z (122)
    0x00, 0x00, 0x3E, 0x04, 0x08, 0x10, 0x3E,
    // { (123)
    0x0C, 0x10, 0x10, 0x30, 0x10, 0x10, 0x0C,
    // | (124)
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    // } (125)
    0x18, 0x04, 0x04, 0x06, 0x04, 0x04, 0x18,
    // ~ (126)
    0x00, 0x00, 0x00, 0x18, 0x24, 0x00, 0x00,
    // DEL (127)
    0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E
};

// Default font definition
static const font_t default_font = {
    .width = 5,
    .height = 7,
    .data = font_5x7
};

void grid_init(void) {
    INFO("grid_init()");
}

// Create a grid object
grid_t *grid_create(strip_t *strip, uint32_t width, uint32_t height, bool serpentine) {
    grid_t *grid = heap_caps_calloc(1, sizeof(grid_t), MALLOC_CAP_DEFAULT);
    if (!grid) {
        ERR("Failed to allocate grid");
        return NULL;
    }

    grid->strip = strip;
    grid->width = width;
    grid->height = height;
    grid->serpentine = serpentine;

    return grid;
}

// Convert grid coordinates to strip index
uint32_t grid_xy_to_index(grid_t *grid, uint32_t x, uint32_t y) {
    if (x >= grid->width || y >= grid->height) {
        return 0; // Out of bounds
    }

    uint32_t index;
    if (grid->serpentine) {
        // For serpentine pattern, reverse direction for odd columns
        if (x % 2 == 0) {
            index = x * grid->height + y;
        } else {
            index = x * grid->height + (grid->height - 1 - y);
        }
    } else {
        // Standard vertical-first pattern
        index = x * grid->height + y;
    }

    return index;
}

// Set a pixel in the grid
esp_err_t grid_set_pixel(grid_t *grid, uint32_t x, uint32_t y, color_t color) {
    uint32_t index = grid_xy_to_index(grid, x, y);
    return strip_set_pixel_rgb(grid->strip, index, color.r, color.g, color.b);
}

// Clear the grid
esp_err_t grid_clear(grid_t *grid) {
    return strip_set_all_rgb(grid->strip, 0, 0, 0);
}

// Draw a line using Bresenham's algorithm
esp_err_t grid_draw_line(grid_t *grid, int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t color) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    int16_t e2;

    while (1) {
        // Only draw if within bounds
        if (x0 >= 0 && x0 < grid->width && y0 >= 0 && y0 < grid->height) {
            grid_set_pixel(grid, x0, y0, color);
        }

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }

    return ESP_OK;
}

// Draw a rectangle outline
esp_err_t grid_draw_rect(grid_t *grid, int16_t x, int16_t y, uint16_t width, uint16_t height, color_t color) {
    // Draw horizontal lines
    grid_draw_line(grid, x, y, x + width - 1, y, color);
    grid_draw_line(grid, x, y + height - 1, x + width - 1, y + height - 1, color);
    
    // Draw vertical lines
    grid_draw_line(grid, x, y, x, y + height - 1, color);
    grid_draw_line(grid, x + width - 1, y, x + width - 1, y + height - 1, color);
    
    return ESP_OK;
}

// Fill a rectangle
esp_err_t grid_fill_rect(grid_t *grid, int16_t x, int16_t y, uint16_t width, uint16_t height, color_t color) {
    for (int16_t i = x; i < x + width; i++) {
        for (int16_t j = y; j < y + height; j++) {
            if (i >= 0 && i < grid->width && j >= 0 && j < grid->height) {
                grid_set_pixel(grid, i, j, color);
            }
        }
    }
    return ESP_OK;
}

// Draw a circle outline using Bresenham's circle algorithm
esp_err_t grid_draw_circle(grid_t *grid, int16_t x0, int16_t y0, uint16_t radius, color_t color) {
    int16_t x = 0;
    int16_t y = radius;
    int16_t d = 3 - 2 * radius;
    
    while (y >= x) {
        // Draw the eight points of the circle
        if (x0 + x >= 0 && x0 + x < grid->width && y0 + y >= 0 && y0 + y < grid->height)
            grid_set_pixel(grid, x0 + x, y0 + y, color);
        if (x0 - x >= 0 && x0 - x < grid->width && y0 + y >= 0 && y0 + y < grid->height)
            grid_set_pixel(grid, x0 - x, y0 + y, color);
        if (x0 + x >= 0 && x0 + x < grid->width && y0 - y >= 0 && y0 - y < grid->height)
            grid_set_pixel(grid, x0 + x, y0 - y, color);
        if (x0 - x >= 0 && x0 - x < grid->width && y0 - y >= 0 && y0 - y < grid->height)
            grid_set_pixel(grid, x0 - x, y0 - y, color);
        if (x0 + y >= 0 && x0 + y < grid->width && y0 + x >= 0 && y0 + x < grid->height)
            grid_set_pixel(grid, x0 + y, y0 + x, color);
        if (x0 - y >= 0 && x0 - y < grid->width && y0 + x >= 0 && y0 + x < grid->height)
            grid_set_pixel(grid, x0 - y, y0 + x, color);
        if (x0 + y >= 0 && x0 + y < grid->width && y0 - x >= 0 && y0 - x < grid->height)
            grid_set_pixel(grid, x0 + y, y0 - x, color);
        if (x0 - y >= 0 && x0 - y < grid->width && y0 - x >= 0 && y0 - x < grid->height)
            grid_set_pixel(grid, x0 - y, y0 - x, color);
        
        x++;
        
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
    
    return ESP_OK;
}

// Fill a circle
esp_err_t grid_fill_circle(grid_t *grid, int16_t x0, int16_t y0, uint16_t radius, color_t color) {
    int16_t x = 0;
    int16_t y = radius;
    int16_t d = 3 - 2 * radius;
    
    while (y >= x) {
        // Draw horizontal lines to fill the circle
        if (x0 + x >= 0 && x0 + x < grid->width) {
            if (y0 + y >= 0 && y0 + y < grid->height)
                grid_draw_line(grid, x0 - x, y0 + y, x0 + x, y0 + y, color);
            if (y0 - y >= 0 && y0 - y < grid->height)
                grid_draw_line(grid, x0 - x, y0 - y, x0 + x, y0 - y, color);
        }
        
        if (x0 + y >= 0 && x0 + y < grid->width) {
            if (y0 + x >= 0 && y0 + x < grid->height)
                grid_draw_line(grid, x0 - y, y0 + x, x0 + y, y0 + x, color);
            if (y0 - x >= 0 && y0 - x < grid->height)
                grid_draw_line(grid, x0 - y, y0 - x, x0 + y, y0 - x, color);
        }
        
        x++;
        
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
    
    return ESP_OK;
}

// Draw a character
esp_err_t grid_draw_char(grid_t *grid, int16_t x, int16_t y, char c, const font_t *font, color_t color) {
    if (!font) {
        font = &default_font;
    }
    
    // Ensure character is in printable range
    if (c < 32 || c > 127) {
        c = '?';
    }
    
    // Calculate offset into font data
    int char_index = c - 32;
    const uint8_t *char_data = &font->data[char_index * font->height];
    
    // Draw each row of the character
    for (int row = 0; row < font->height; row++) {
        uint8_t row_data = char_data[row];
        
        // Draw each pixel in the row
        for (int col = 0; col < font->width; col++) {
            if (row_data & (0x10 >> col)) {
                int pixel_x = x + col;
                int pixel_y = y + row;
                
                if (pixel_x >= 0 && pixel_x < grid->width && pixel_y >= 0 && pixel_y < grid->height) {
                    grid_set_pixel(grid, pixel_x, pixel_y, color);
                }
            }
        }
    }
    
    return ESP_OK;
}

// Draw a string
esp_err_t grid_draw_string(grid_t *grid, int16_t x, int16_t y, const char *str, const font_t *font, color_t color) {
    if (!str) {
        return ESP_ERR_INVALID_ARG;
    }
    
    int16_t current_x = x;
    
    for (int i = 0; str[i] != '\0'; i++) {
        grid_draw_char(grid, current_x, y, str[i], font, color);
        current_x += font->width + 1; // Add 1 pixel spacing between characters
        
        // Check if we've gone off the edge of the grid
        if (current_x >= grid->width) {
            break;
        }
    }
    
    return ESP_OK;
}

// Refresh the grid (update the strip)
esp_err_t grid_refresh(grid_t *grid) {
    return strip_refresh(grid->strip);
}

// Animation helper for scrolling text
esp_err_t grid_scroll_text(grid_t *grid, const char *text, const font_t *font, color_t color, int16_t start_y, int16_t speed) {
    static int16_t scroll_x = GRID_WIDTH;
    static uint32_t last_update = 0;
    uint32_t current_time = esp_timer_get_time() / 1000; // Convert to milliseconds
    
    // Check if it's time to update
    if (current_time - last_update < speed) {
        return ESP_OK;
    }
    
    last_update = current_time;
    
    // Clear the grid
    grid_clear(grid);
    
    // Draw the text at the current position
    grid_draw_string(grid, scroll_x, start_y, text, font, color);
    
    // Move the text to the left
    scroll_x--;
    
    // Reset position when text is completely off the screen
    int text_width = strlen(text) * (font->width + 1);
    if (scroll_x < -text_width) {
        scroll_x = GRID_WIDTH;
    }
    
    // Refresh the grid
    grid_refresh(grid);
    
    return ESP_OK;
}

*/