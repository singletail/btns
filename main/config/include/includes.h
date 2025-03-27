#ifndef _INCLUDES_H
#define _INCLUDES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <inttypes.h>
#include <string.h>

#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "rom/ets_sys.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "defines.h"
#include "config.h"

#include "sym.h"
#include "log.h"
#include "bits.h"
#include "pins.h"
#include "init.h"

#include "net.h"
#include "ntp.h"
#include "mqtt.h"
#include "ha.h"
#include "perf.h"

#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_types.h"
#include "driver/i2c_master.h"
#include "soc/clk_tree_defs.h"
#include "ws2812.h"

#endif