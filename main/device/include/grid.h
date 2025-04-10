#ifndef GRID_H
#define GRID_H

#include "strip.h"

// Grid dimensions
#define GRID_WIDTH 64
#define GRID_HEIGHT 8
#define GRID_TOTAL_PIXELS (GRID_WIDTH * GRID_HEIGHT)

// Color structure
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

// Grid structure
typedef struct grid_t {
    strip_t *strip;
    uint32_t width;
    uint32_t height;
    bool serpentine;  // Whether the grid uses serpentine pattern
} grid_t;

// Font structure
typedef struct {
    uint8_t width;
    uint8_t height;
    const uint8_t *data;  // Bitmap data for the font
} font_t;


// Create a grid object
grid_t *grid_create(strip_t *strip, uint32_t width, uint32_t height, bool serpentine);

// Convert grid coordinates to strip index
uint32_t grid_xy_to_index(grid_t *grid, uint32_t x, uint32_t y);

// Set a pixel in the grid
esp_err_t grid_set_pixel(grid_t *grid, uint32_t x, uint32_t y, color_t color);

// Clear the grid
esp_err_t grid_clear(grid_t *grid);

// Draw a line
esp_err_t grid_draw_line(grid_t *grid, int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t color);

// Draw a rectangle
esp_err_t grid_draw_rect(grid_t *grid, int16_t x, int16_t y, uint16_t width, uint16_t height, color_t color);

// Fill a rectangle
esp_err_t grid_fill_rect(grid_t *grid, int16_t x, int16_t y, uint16_t width, uint16_t height, color_t color);

// Draw a circle
esp_err_t grid_draw_circle(grid_t *grid, int16_t x0, int16_t y0, uint16_t radius, color_t color);

// Fill a circle
esp_err_t grid_fill_circle(grid_t *grid, int16_t x0, int16_t y0, uint16_t radius, color_t color);

// Draw a character
esp_err_t grid_draw_char(grid_t *grid, int16_t x, int16_t y, char c, const font_t *font, color_t color);

// Draw a string
esp_err_t grid_draw_string(grid_t *grid, int16_t x, int16_t y, const char *str, const font_t *font, color_t color);

// Refresh the grid (update the strip)
esp_err_t grid_refresh(grid_t *grid);

// Animation helpers
esp_err_t grid_scroll_text(grid_t *grid, const char *text, const font_t *font, color_t color, int16_t start_y, int16_t speed);

#endif
