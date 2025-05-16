#ifndef LEDS_H
#define LEDS_H

#include "includes.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} hsv_t;

void hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b);
void letter_test(void *arg);
void hsv_task(void *arg);
void grid_task(void *arg);
void cylon_task(void *arg);
void v_task(void *arg);
void pulse_task(void *arg);
void police_alarm_task(void *arg);
void police_task(void *arg);
void federal_task(void *arg);
void maybe_task(void *arg);
#endif // LEDS_H
