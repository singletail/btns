#include "init.h"

void init_devices(void) {
    device_init();
}

void init_init(void) {
    YES("init_init(init)");

#ifdef PIN_MMWAVE
    mmwave_init();
#endif

#ifdef PIN_SEG7_CLK
    INFO("seg7_init()");
    seg7_t *display = seg7_new(PIN_SEG7_CLK, PIN_SEG7_DIO);
    if (display) {
        INFO("seg7_init() success");
        INFO("sending clear");
        seg7_clear(display);

        INFO("sending display 1234");
        seg7_display_number(display, 5678);
        INFO("display command sent");
    } else {
        ERR("seg7_init() failed");
    }
#endif

#ifdef PIN_LED_1
    strip_t *strip_main =
        strip_create(PIN_LED_1, LED_LENGTH_1, true, STRIP_TYPE_MAIN);
    if (strip_main) {
        xTaskCreate(strip_cycle_task, // Task function
            //"strip_cycle_task",       // Name
            "strip_chase_task",       // Name
            4096,                     // Stack size
            (void *)strip_main,       // Parameter to pass (cast to void*)
            5,                        // Priority
            NULL);                    // Task handle (if needed)

    } else {
        ERR("strip_main not created");
    }
#endif
}
