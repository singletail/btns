#ifndef _INCLUDE_H
#define _INCLUDE_H

// C
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

// Setup
#include "config.h"
#include "define.h"
#include "device.h"
#include "init.h"

// System
#include "nvs_flash.h"
#include "rom/ets_sys.h"

// HAL
#include "hal/gpio_types.h"

// GPIO
#include "driver/gpio.h"
#include "hal/gpio_types.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

// ESP
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"

// Core
#include "bits.h"
#include "nvs.h"
#include "timex.h"

// Old - to redo:
#include "err.h"
#include "log.h"
#include "pin.h" // replace?
#include "sym.h"
#include "tick.h"

// NTP
#include "esp_sntp.h"

// NET
#include "ctrl_net.h"
#include "net_evt.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

// MQTT
#include "broker.h"
#include "mqtt_client.h"

// SENSOR
#include "ha.h"
#include "device.h"
#include "sensor.h"

// SEG7
#include "seg7.h"

// LED
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_types.h"
#include "encoder.h"
#include "soc/clk_tree_defs.h"
#include "strip.h"

// MMWAVE
#include "mmwave.h"

#endif
