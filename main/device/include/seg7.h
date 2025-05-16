#ifndef _SEG7_H
#define _SEG7_H

#include "esp_system.h"
#include "hal/gpio_ll.h"
#include "includes.h"
#include "soc/gpio_struct.h"

typedef struct {
    gpio_dev_t *hw;
    gpio_num_t clk;
    gpio_num_t dio;
    uint8_t brightness;
} seg7_t;

seg7_t *seg7_new(gpio_num_t clk, gpio_num_t dio);
void seg7_del(seg7_t *dev);

// Display control
void seg7_set_brightness(seg7_t *dev, uint8_t level);
void seg7_display(seg7_t *dev, uint8_t addr, uint8_t data);
void seg7_clear(seg7_t *dev);

// Optional: Higher level helpers
void seg7_display_digit(seg7_t *dev, uint8_t position, uint8_t digit);
void seg7_display_number(seg7_t *dev, int16_t number);

void clock_task(void *arg);
void wargames_task(void *arg);

#endif
