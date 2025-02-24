#ifndef _NTP_H
#define _NTP_H

#include "includes.h"
#include "esp_sntp.h"

void ntp_init(void);
uint64_t ntp_ms(void);
char *ntp_ts(void);
void ntp_print(void);
void ntp_callback(struct timeval *tv);

#endif
