#include "init.h"
#include "leds.h"
void init() {
    INFO("init()");

#ifdef PIN_SEG7_CLK
    xTaskCreate(clock_task, "clock", 2048, NULL, 2, NULL);
#endif

    xTaskCreate(letter_test, "letter", 8192, NULL, 2, NULL);

    INFO("Done.");
}