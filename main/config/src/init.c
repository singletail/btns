#include "init.h"

void init() {
    INFO("init()");

    strip_t *strip_main = strip_create(PIN_LED_1, LED_LENGTH_1, true, STRIP_TYPE_MAIN);
    if (strip_main) {
        INFO("creating task");
        xTaskCreate(
            strip_rainbow_task,       // Task function
            "strip_rainbow_task",     // Name
            4096,                  // Stack size
            (void *)strip_main,    // Parameter to pass (cast to void*)
            5,                     // Priority
            NULL);                 // Task handle (if needed)

    } else {
        ERR("strip_main not created");
    }
}