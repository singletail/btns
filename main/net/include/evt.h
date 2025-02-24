#ifndef _EVT_H
#define _EVT_H

#include "net.h"
#include "includes.h"

void evt_wifi(void *arg, esp_event_base_t evt_base, int32_t evt_id, void *evt_data);
void evt_wifi_ip(void *arg, esp_event_base_t evt_base, int32_t evt_id, void *event_data);
void evt_add_handlers(esp_netif_t *s_sta_netif);

#endif

