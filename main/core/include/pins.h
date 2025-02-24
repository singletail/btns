#ifndef PINS_H
#define PINS_H

#include "includes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"

typedef void (*pins_callback_t)(uint32_t pin);

typedef struct {
    gpio_num_t pin;
    int val;
    uint64_t last;
    pins_callback_t callback;
} pins_obj_t;

void pins_init(void);
esp_err_t pins_config_in(
    gpio_num_t pin, bool pullup, bool pulldown, gpio_int_type_t intr, pins_callback_t callback);
esp_err_t pins_config_out(gpio_num_t pin, bool pullup, bool pulldown, gpio_int_type_t intr);
esp_err_t pins_write(gpio_num_t pin, int val);
int pins_read(gpio_num_t pin);

#endif
