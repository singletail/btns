#ifndef _BITS_H
#define _BITS_H

#include "includes.h"
#include "freertos/FreeRTOS.h"

void bits_set(EventBits_t bits);
void bits_clr(EventBits_t bits);
void bits_wait(EventBits_t bit);
int bits_int(EventBits_t bit);
bool bits_bool(EventBits_t bit);
void bits_debug(EventBits_t bit);
void bits_init(void);

#endif
