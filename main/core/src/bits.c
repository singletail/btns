#include "bits.h"

static EventGroupHandle_t ctrl_bits;

// #define ALL_BITS ((1 << 24) - 1) // 0x00FFFFFF
static const EventBits_t ALL_BITS = ((1 << 24) - 1); // 0x00FFFFFF


/*     For debugging:      */

enum { EVENT_BITS = 24 };
static char bits_char[EVENT_BITS + 1] = {0};
static char bit_char[2] = {0};
static bool debug = false;

static void bits_reset(void) {
    xEventGroupClearBits(ctrl_bits, ALL_BITS);
}

static void bits_char_update(void) {
    EventBits_t bits = xEventGroupGetBits(ctrl_bits);
    for (int i = EVENT_BITS - 1; i >= 0; i--) {
        bits_char[i] = (bits & (1 << i)) ? '1' : '0';
    }
    bits_char[EVENT_BITS] = '\0';
}

static void bit_char_update(EventBits_t bit) {
    EventBits_t bits = xEventGroupGetBits(ctrl_bits);
    sprintf(bit_char, "%d", (bits & bit) ? 1 : 0);
}

static void bits_print(void) {
    bits_char_update();
    printf("%s[%s]: %s%s\n", col_o, sym_bits, bits_char, col_reset);
}

static void bits_print_bit(EventBits_t bit) {
    bit_char_update(bit);
    printf("%s[%s]:  %s%s\n", col_y, sym_bits, bit_char, col_reset);
}

/*     End of debugging:      */

void bits_init(void) {
    LOG("bits_init()");
    ctrl_bits = xEventGroupCreate();
    bits_reset();
    bits_print();
}

// -----------------   Methods   --------------------

void bits_on(EventBits_t bits) {
    xEventGroupSetBits(ctrl_bits, bits);
}

void bits_off(EventBits_t bits) {
    xEventGroupClearBits(ctrl_bits, bits);
}

EventBits_t bits_all(void) {
    return xEventGroupGetBits(ctrl_bits);
}

EventBits_t bits_get(EventBits_t bit) {
    return xEventGroupGetBits(ctrl_bits) & bit;
}

void bits_wait(EventBits_t bit) {
    if (debug) {
        printf("%s[%s] waiting for: 0x%08lx%s\n", col_b, sym_bits, bit, col_reset);}
    xEventGroupWaitBits(ctrl_bits, bit, pdFALSE, pdTRUE, portMAX_DELAY);
}

// ---------------- Conversions ----------------------

int bits_int(EventBits_t bit) {
    EventBits_t bits = xEventGroupGetBits(ctrl_bits);
    int result = ((bits & bit) != 0) ? 1 : 0;
    if (debug) {
        printf("%s[%s] bits_get(%s) = %d%s\n", col_p, sym_bits, (bits & bit) ? "1" : "0",
            result, col_reset);}
    return result;
}

bool bits_chk(EventBits_t bit) {
    EventBits_t bits = xEventGroupGetBits(ctrl_bits);
    bool result = ((bits & bit) != 0);
    if (debug) {
        printf("%s[%s] bits_chk(%s) = %s%s\n", col_p, sym_bits, (bits & bit) ? "1" : "0",
            result ? "true" : "false", col_reset);
    }
    return result;
}

// ---------------------- Tests ---------------------------------

void bits_test(void) {
    printf("%s[%s] ----- bits_test() -----\n", col_r, sym_bits);
    bits_print();

    printf("%s___________________________________%s\n", col_b, col_reset);

    printf("%s[%s] bits: setting BIT21)\n", col_r, sym_bits);
    bits_on(BIT21);
    bits_print_bit(BIT21);
    bits_print();

    printf("clear BIT21\n");
    bits_off(BIT21);

    printf("%s[%s] bits: setting BIT22)\n", col_r, sym_bits);
    bits_on(BIT22);
    bits_print_bit(BIT22);
    bits_print();

    printf("clear BIT22\n");
    bits_off(BIT22);

    printf("%s[%s] bits: setting BIT23)\n", col_r, sym_bits);
    bits_on(BIT23);
    bits_print_bit(BIT23);
    bits_print();

    printf("clear BIT23\n");
    bits_off(BIT23);

    printf("%s___________________________________%s\n", col_b, col_reset);

    int test_bit_int = bits_int(BIT_TEST_21);
    printf("%s[%s] bits: bits_int(BIT_TEST_21): %d%s\n", col_y, sym_bits,
        test_bit_int, col_reset);

    bool test_bit_bool = bits_chk(BIT_TEST_21);
    printf("%s[%s] bits: bits_chk(BIT_TEST_21): %s%s\n", col_g, sym_bits,
        test_bit_bool ? "true" : "false", col_reset);
    
    printf("%s___________________________________%s\n", col_b, col_reset);

    printf("%s[%s] bits: clearing BIT_TEST_21)\n", col_r, sym_bits);
    bits_off(BIT_TEST_21);
    bits_print_bit(BIT_TEST_21);
    bits_print();

    test_bit_int = bits_int(BIT_TEST_21);
    printf("%s[%s] bits: bits_int(BIT_TEST_21): %d%s\n", col_y, sym_bits,
        test_bit_int, col_reset);

    test_bit_bool = bits_chk(BIT_TEST_21);
    printf("%s[%s] bits: bits_chk(BIT_TEST_21): %s%s\n", col_g, sym_bits,
        test_bit_bool ? "true" : "false", col_reset);

    printf("%s___________________________________%s\n", col_b, col_reset);

    printf("%s[%s] Bool Comparison: if (bits_chk(BIT_TEST_21)) { printf(\"PASS\") } (should fail): %s", col_y,
        sym_bits, col_reset);

    if (bits_chk(BIT_TEST_21)) {
        printf("%sPASS%s\n", col_r, col_reset);
    } else {
        printf("%sFAIL%s\n", col_g, col_reset);
    }

    printf("%s[%s] Int Comparison: if (bits_int(BIT_TEST_21) == 1) { printf(\"PASS\") } (should fail): %s", col_y,
        sym_bits, col_reset);

    if (bits_int(BIT_TEST_21) == 1) {
        printf("%sPASS%s\n", col_r, col_reset);
    } else {
        printf("%sFAIL%s\n", col_g, col_reset);
    }

    printf("%s___________________________________%s\n", col_b, col_reset);

    printf("%s[%s] Setting BIT_TEST_21 to ON.%s\n", col_g, sym_bits, col_reset);
    bits_on(BIT_TEST_21);
    bits_print_bit(BIT_TEST_21);
    bits_print();
    
    printf("%s[%s] Bool Comparison: if (bits_chk(BIT_TEST_21)) { printf(\"PASS\") } (should pass): %s", col_y,
        sym_bits, col_reset);

    if (bits_chk(BIT_TEST_21)) {
        printf("%sPASS%s\n", col_g, col_reset);
    } else {
        printf("%sFAIL%s\n", col_r, col_reset);
    }

    printf("%s[%s] Int Comparison: if (bits_int(BIT_TEST_21) == 1) { printf(\"PASS\") } (should pass): %s", col_y,
        sym_bits, col_reset);

    if (bits_int(BIT_TEST_21) == 1) {
        printf("%sPASS%s\n", col_g, col_reset);
    } else {
        printf("%sFAIL%s\n", col_r, col_reset);
    }

    printf("%s___________________________________%s\n", col_b, col_reset);

    printf("%s[%s] --- bits_demo() done ---%s\n", col_r, sym_bits, col_reset);
}

/* examples:
xEventGroupSetBits(ctrl_bits, NET_OK);
xEventGroupClearBits(ctrl_bits, NET_OK);
xEventGroupWaitBits(ctrl_bits, NET_OK, pdFALSE, pdTRUE, portMAX_DELAY);
(xEventGroupGetBits(ctrl_bits) & NET_OK) != 0
*/