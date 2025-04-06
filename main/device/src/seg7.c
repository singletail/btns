#include "seg7.h"

/*

       a
    ______
    |     |
  f |     | b
    |__g__|
    |     |
  e |     | c
    |_____|
       d
*/
/*
static const int8_t seg7_sym[] = {
    // XGFEDCBA
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b00111001, // C
    0b01111001, // E
    0b01110001, // F
    0b01110110, // H
    0b00111000, // L
    0b01110011, // P
    0b01101101, // S
    0b00111110, // U
};

static const int8_t seg7_bits[] = {
    // XGFEDCBA
    0b00000001, // 1
    0b00000010, // 2
    0b00000100, // 4
    0b00001000, // 8
    0b00010000, // 16
    0b00100000, // 32
};

#include <sys/time.h>
#include <time.h>



uint8_t seg7_hex(const uint8_t seg) {
    return seg;
}


*/

static inline void clk_high(seg7_t *dev) { gpio_ll_set_level(dev->hw, dev->clk, 1); }

static inline void clk_low(seg7_t *dev) { gpio_ll_set_level(dev->hw, dev->clk, 0); }

static inline void dio_set(seg7_t *dev, uint32_t level) {
    gpio_ll_set_level(dev->hw, dev->dio, level);
}

static inline void dio_high(seg7_t *dev) { gpio_ll_set_level(dev->hw, dev->dio, 1); }

static inline void dio_low(seg7_t *dev) { gpio_ll_set_level(dev->hw, dev->dio, 0); }

static inline void dio_out(seg7_t *dev) {
    gpio_ll_input_disable(dev->hw, dev->dio);
    gpio_ll_output_enable(dev->hw, dev->dio);
}

static inline void dio_in(seg7_t *dev) {
    gpio_ll_output_disable(dev->hw, dev->dio);
    gpio_ll_input_enable(dev->hw, dev->dio);
}

static inline void clk_out(seg7_t *dev) {
    gpio_ll_input_disable(dev->hw, dev->clk);
    gpio_ll_output_enable(dev->hw, dev->clk);
}

static inline void clk_in(seg7_t *dev) {
    gpio_ll_output_disable(dev->hw, dev->clk);
    gpio_ll_input_enable(dev->hw, dev->clk);
}

static inline void delay() { ets_delay_us(2); }

// Protocol implementation

static void start(seg7_t *dev) {
    dio_low(dev);
    delay();
}

static void stop(seg7_t *dev) {
    clk_low(dev);    // assumed?
    delay();
    dio_low(dev);
    delay();
    clk_high(dev);
    delay();
    dio_high(dev);
    delay();
}

static void ask(seg7_t *dev) {
    dio_in(dev);
    clk_low(dev);

    delay();
    delay();

    clk_high(dev);
    delay();
    clk_low(dev);
    dio_out(dev);
}

static void write_byte(seg7_t *dev, uint8_t data) {
    uint8_t i;
        for (i = 0; i < 8; i++) {
            clk_low(dev);
            delay();
            dio_set(dev, data & 0x01);
            data >>= 1;
            delay();
            clk_high(dev);
            delay();
        }
}

// ###########  New Instance  ############# //

seg7_t *seg7_new(gpio_num_t clk, gpio_num_t dio) {
    seg7_t *dev = heap_caps_calloc(1, sizeof(seg7_t), MALLOC_CAP_DEFAULT);
    if (!dev) return NULL;

    dev->hw = &GPIO;
    dev->clk = clk;
    dev->dio = dio;
    dev->brightness = 7;

    clk_out(dev);
    clk_low(dev);
    delay();
    dio_out(dev);
    dio_high(dev);
    delay();
    clk_high(dev);
    delay();

    return dev;
}

// ###########  Display  ############# //

void seg7_display(seg7_t *dev, uint8_t addr, uint8_t data) {
    start(dev);
    write_byte(dev, 0x44);    // Fixed address
    ask(dev);
    stop(dev);

    start(dev);
    write_byte(dev, 0xC0 | addr);
    ask(dev);

    write_byte(dev, data);
    ask(dev);
    stop(dev);

    start(dev);
    write_byte(dev, 0x88 | dev->brightness);
    ask(dev);
    stop(dev);
}

// ############################## //

// Segment patterns for digits 0-9
static const uint8_t DIGIT_PATTERNS[] = {
    0x3f,    // 0: 0b00111111
    0x06,    // 1: 0b00000110
    0x5b,    // 2: 0b01011011
    0x4f,    // 3: 0b01001111
    0x66,    // 4: 0b01100110
    0x6d,    // 5: 0b01101101
    0x7d,    // 6: 0b01111101
    0x07,    // 7: 0b00000111
    0x7f,    // 8: 0b01111111
    0x6f     // 9: 0b01101111
};

void seg7_del(seg7_t *dev) {
        if (dev) {
            heap_caps_free(dev);
    }
}

void seg7_set_brightness(seg7_t *dev, uint8_t level) {
    dev->brightness = level & 0x07;    // Ensure 0-7 range
    // Update display with new brightness
    start(dev);
    write_byte(dev, 0x88 | dev->brightness);
    ask(dev);
    stop(dev);
}

void seg7_clear(seg7_t *dev) {
        for (uint8_t i = 0; i < 4; i++) {
            seg7_display(dev, i, 0x00);
        }
}

void seg7_display_digit(seg7_t *dev, uint8_t position, uint8_t digit) {
    if (position > 3 || digit > 9) return;
    seg7_display(dev, position, DIGIT_PATTERNS[digit]);
}

void seg7_display_number(seg7_t *dev, int16_t number) {
    bool negative = number < 0;
        if (negative) {
            number = -number;
    }

    // Convert to digits
    uint8_t digits[4] = {0};
    uint8_t pos = 0;

        // Handle zero case
        if (number == 0) {
            seg7_display_digit(dev, 3, 0);
            return;
    }

        // Extract digits
        while (number > 0 && pos < 4) {
            digits[pos++] = number % 10;
            number /= 10;
        }

        // Display digits
        for (int8_t i = 0; i < pos; i++) {
                if (negative && i == pos - 1) {
                    // Display minus sign
                    seg7_display(dev, 3 - i, 0x40);
                } else {
                    seg7_display_digit(dev, 3 - i, digits[i]);
                }
        }
}

void clock_task(void *arg) {
    LOG("clock_task()");
    
    // Create 7-segment display instance
    seg7_t *display = seg7_new(PIN_SEG7_CLK, PIN_SEG7_DIO);
    if (!display) {
        WARN("Failed to create 7-segment display");
        vTaskDelete(NULL);
        return;
    }
    
    // Set initial brightness
    seg7_set_brightness(display, 6);
    
    time_t now;
    struct tm timeinfo;
    bool colon_state = false;
    uint32_t last_update = 0;
    const uint32_t COLON_BLINK_MS = 500;  // Blink every 500ms
    
    // Wait for NTP sync
    bits_wait(BIT_NTP_READY);
    
    while (1) {
        time(&now);
        localtime_r(&now, &timeinfo);
        
        // Get current time in ms
        uint32_t current_ms = ntp_ms() % 1000;
        
        // Update display only if needed (every 500ms for colon blink)
        if (current_ms - last_update >= COLON_BLINK_MS || current_ms < last_update) {
            last_update = current_ms;
            colon_state = !colon_state;
            
            // Display hours (00-23)
            seg7_display_digit(display, 0, timeinfo.tm_hour / 10);
            seg7_display_digit(display, 1, timeinfo.tm_hour % 10);
            
            // Display minutes with colon
            // seg7_display(display, 1, seg7_display_digit(display, 1, timeinfo.tm_hour % 10) | (colon_state ? 0x80 : 0x00));
            seg7_display_digit(display, 2, timeinfo.tm_min / 10);
            seg7_display_digit(display, 3, timeinfo.tm_min % 10);
        }
        
        // Sleep for ~100ms - short enough to catch time changes but long enough to be efficient
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}