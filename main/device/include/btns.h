#ifndef BTNS_H
#define BTNS_H

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

// Button event types
typedef enum {
    BTN_EVENT_PRESSED,
    BTN_EVENT_RELEASED,
    BTN_EVENT_CLICKED,
    BTN_EVENT_LONG_PRESSED
} btn_event_t;

// Callback function type for button events
typedef void (*btn_callback_t)(void *arg, btn_event_t event);

// Button configuration
typedef struct {
    gpio_num_t pin;            // Button GPIO pin
    gpio_num_t led_pin;        // Optional LED GPIO pin (GPIO_NUM_NC if not used)
    uint32_t debounce_ms;      // Debounce time in milliseconds (recommend 50ms)
    uint32_t long_press_ms;    // Long press time in milliseconds (recommend 1000ms)
    bool active_low;           // If true, button is active low (common for pull-up configs)
    btn_callback_t callback;   // Callback for all button events
    void *user_data;           // User data passed to callbacks
} btn_config_t;

// Button handle
typedef struct btn_handle_s *btn_handle_t;

// Initialize button module
esp_err_t btn_init(void);

// Create a new button instance
esp_err_t btn_create(const btn_config_t *config, btn_handle_t *handle_out);

// Delete a button instance
esp_err_t btn_delete(btn_handle_t handle);

// Set LED state for a button
esp_err_t btn_set_led(btn_handle_t handle, bool state);

// Get current button state (debounced)
bool btn_is_pressed(btn_handle_t handle);

#endif


