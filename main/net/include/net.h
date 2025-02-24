#ifndef _NET_H
#define _NET_H

#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_wifi.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "includes.h"
#include "evt.h"

void net_wifi_connect(void);
void net_init(void);

#endif

