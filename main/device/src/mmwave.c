#include "mmwave.h"

#ifdef PIN_MMWAVE

static const uint32_t pin_led = PIN_MMWAVE_LED;
static const uint32_t pin_sensor = PIN_MMWAVE;

static void mmwave_led(void) {
    if (pin_led) pin_write(pin_led, bits_chk(BIT_PRESENCE_DETECTED));
}

static void mmwave_check_val(int value) {
    int oldvalue = bits_int(BIT_PRESENCE_DETECTED);
    if (value == oldvalue) return;
        if (value == 1) {
            printf("mmwave presence detected\n");
            bits_on(BIT_PRESENCE_DETECTED);
        } else {
            printf("mmwave presence cleared\n");
            bits_off(BIT_PRESENCE_DETECTED);
        }
    mmwave_led();
    sensor_update_bool(0, value == 1);
    if (bits_chk(BIT_HA_OK)) ha_publish_state();
}

void mmwave_callback(uint32_t pin) {
    static uint32_t last_trigger = 0;
    uint32_t now = esp_timer_get_time() / 1000;    // Convert to ms
        if (now - last_trigger < 100) {            // 100ms debounce
            return;
    }
    last_trigger = now;

    int value = pin_read(pin);
    mmwave_check_val(value);
}

int mmwave_read(void) {
    int value = pin_read(pin_sensor);
    mmwave_check_val(value);
    return value;
}

void mmwave_init(void) {
        if (!pin_sensor) {
            return;
    }
    bits_wait(BIT_SENSOR_READY);
    pin_config_in(pin_sensor, false, false, GPIO_INTR_ANYEDGE, mmwave_callback);
    if (pin_led) pin_config_out(PIN_MMWAVE_LED, false, false, GPIO_INTR_DISABLE);
    mmwave_read();
}
#endif