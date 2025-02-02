#include "include.h"

void app_main(void) {
    YES("app_main()");
    nvs_init();
    bits_init();
    pin_init();
    init_devices();
    ctrl_net_init();
    tick_init();



    init_init();
    // ctrl_console_init();
    bits_test();
    LOG("init done.\n");
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
}
