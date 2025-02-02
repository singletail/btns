#ifndef __CTRL_NET_EVENT_H
#define __CTRL_NET_EVENT_H

// #ifdef ENABLE_NET

#include "esp_netif_types.h"
#include "include.h"

void net_evt_wifi(
    void *arg, esp_event_base_t evt_base, int32_t evt_id, void *evt_data);
void net_evt_wifi_ip(
    void *arg, esp_event_base_t evt_base, int32_t evt_id, void *event_data);
void net_evt_add_handlers(esp_netif_t *s_sta_netif);

// #endif

#endif
