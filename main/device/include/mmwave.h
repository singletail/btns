#ifndef _mmwave_H
#define _mmwave_H

#include "include.h"
#include "pin.h"

void mmwave_callback(uint32_t pin);
int mmwave_read(void);
void mmwave_init(void);

#endif
