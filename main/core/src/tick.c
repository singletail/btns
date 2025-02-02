#include "tick.h"

TaskHandle_t tickTaskHandle = NULL;

const static uint64_t TICK_THROTTLE = 5000;
const static uint32_t LOOP_THROTTLE = 500;

void tick_init(void) {
    xTaskCreate(tick_task, "tick_task", DEFAULT_TASK_SIZE, NULL, 5, &tickTaskHandle);
}

void tick_report(void) {
    print_timex();
    int mmwave = mmwave_read();
    printf("mmwave: %d\n", mmwave);
}

void tick_task(void *parameter) {
    uint64_t tick_last = 0;
    uint64_t tick_throttle = TICK_THROTTLE;

#ifdef PIN_BLINK
    uint32_t led_throttle = 200;
    uint64_t led_last = 0;
    uint8_t led_state = 0;
#endif

    while (1) {
        uint64_t now = timex_ms();
        if (now > tick_last + tick_throttle) {
            tick_last = now;
            tick_report();
        }

#ifdef PIN_BLINK
        if (now - led_last > led_throttle) {
            led_last = now;
            led_state = !led_state;
            gpio_pin_set(PIN_BLINK, led_state);
        }
#endif
        vTaskDelay(LOOP_THROTTLE / portTICK_PERIOD_MS);
    }
}
