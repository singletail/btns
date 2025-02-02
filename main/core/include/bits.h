#ifndef _CTRL_BITS_H
#define _CTRL_BITS_H

#include "include.h"

void bits_init(void);

void bits_on(EventBits_t bits);
void bits_off(EventBits_t bits);

EventBits_t bits_all(void);
EventBits_t bits_get(EventBits_t bit);

bool bits_chk(EventBits_t bits);
int bits_int(EventBits_t bit);

void bits_wait(EventBits_t bit);
void bits_test();

#endif
