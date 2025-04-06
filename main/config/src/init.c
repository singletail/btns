#include "init.h"

void init() {
    INFO("init()");

#ifdef PIN_SEG7_CLK
    xTaskCreate(clock_task, "clock", 2048, NULL, 2, NULL);
#endif

    xTaskCreate(v_task, "v", 2048, NULL, 2, NULL);

    INFO("Done.");
}