#include "nvs.h"
#include "bits.h"
#include "pins.h"
#include "net.h"
#include <stdio.h>

void app_main(void) {
    nvs_init();
    bits_init();
    pins_init();

    #ifdef ENABLE_WIFI
    net_init();
    #endif
    
    init();
}
