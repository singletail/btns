#ifndef PIN_H
#define PIN_H

#include "include.h"

typedef void (*pin_callback_t)(uint32_t pin);

// pin object struct
typedef struct {
    gpio_num_t pin;
    int val;
    uint64_t last;
    pin_callback_t callback;
} pin_obj_t;

void pin_init(void);
esp_err_t pin_config_in(gpio_num_t pin, bool pullup, bool pulldown,
    gpio_int_type_t intr, pin_callback_t callback);
esp_err_t pin_config_out(gpio_num_t pin, bool pullup, bool pulldown,
    gpio_int_type_t intr);
esp_err_t pin_write(gpio_num_t pin, int val);
int pin_read(gpio_num_t pin);

#endif
