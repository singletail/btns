#include "freertos/FreeRTOS.h"
#include <stdio.h>
#include "bits.h"

EventGroupHandle_t ctrl_bits;

enum
{
    EVENT_BITS = 24,
    ALL_BITS = ((1 << 24) - 1)
};

static char bits_char[EVENT_BITS + 1] = {0};
static char bit_char[2] = {0};

static void bits_reset(void) { xEventGroupClearBits(ctrl_bits, ALL_BITS); }

static void bits_char_update(void) {
    EventBits_t bits = xEventGroupGetBits(ctrl_bits);
    for (int i = 0; i < EVENT_BITS; i++) {
        bits_char[i] = (bits & (1 << i)) ? '1' : '0';}
    bits_char[EVENT_BITS] = '\0';
}

static void bit_char_update(EventBits_t bit) {
    bit_char[0] = (xEventGroupGetBits(ctrl_bits) & bit) ? '1' : '0';
    bit_char[1] = '\0';
}

static void bits_print(void) {
    bits_char_update();
    printf("= bits: %s\n", bits_char);
}

static void bit_print(EventBits_t bit) {
    bit_char_update(bit);
    printf("- bit %lu: %s\n", (unsigned long)bit, bit_char);
}

void bits_set(EventBits_t bits) { xEventGroupSetBits(ctrl_bits, bits); }

void bits_clr(EventBits_t bits) { xEventGroupClearBits(ctrl_bits, bits); }

void bits_wait(EventBits_t bit) {
    xEventGroupWaitBits(ctrl_bits, bit, pdFALSE, pdTRUE, portMAX_DELAY);
}

int bits_int(EventBits_t bit) {
    return (int) ((xEventGroupGetBits(ctrl_bits) & bit) != 0) ? 1 : 0;
}

bool bits_bool(EventBits_t bit) {
    return (bool) ((xEventGroupGetBits(ctrl_bits) & bit) != 0);
}

void bits_debug(EventBits_t bit) {
    bit_print(bit);
    printf("- bool: %s\n", (bits_bool(bit) ? "true" : "false" ));
    printf("- int: %d\n", bits_int(bit));
    bits_print();
}

void bits_init(void) {
    INFO("bits_init()");
    ctrl_bits = xEventGroupCreate();
    bits_reset();
    bits_set(BITS_READY);
    bits_wait(BITS_READY);
}

