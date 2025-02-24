#include "pins.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"

#define PINS_QUEUE_SIZE 10
#define PINS_TASK_STACK 4096
#define PINS_TASK_PRIORITY 1
#define PINS_DEBOUNCE_MS 100

static QueueHandle_t pins_queue_h = NULL;
static TaskHandle_t pins_task_h = NULL;
static pins_obj_t pins[SOC_GPIO_PIN_COUNT] = {0};

static void IRAM_ATTR pins_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(pins_queue_h, &gpio_num, NULL);
}

static void pins_fire_callback(uint32_t pin) {
    pins_obj_t *obj = &pins[pin];
        if (obj->callback) {
            obj->callback(pin);
    }
}

static void pins_task(void *arg) {
    uint32_t pin;
        while (1) {
                if (xQueueReceive(pins_queue_h, &pin, portMAX_DELAY)) {
                    pins_fire_callback(pin);
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
}

static esp_err_t pins_add_callback(gpio_num_t pin, pins_callback_t callback) {
    pins_obj_t *pin_obj = &pins[pin];
    memset(pin_obj, 0, sizeof(pins_obj_t));
    pin_obj->pin = pin;
    pin_obj->val = 0;
    pin_obj->last = 0;
    pin_obj->callback = callback;
    return ESP_OK;
}

void pins_init(void) {
    INFO("pins_init()");
    pins_queue_h = xQueueCreate(PINS_QUEUE_SIZE, sizeof(int));
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    xTaskCreate(pins_task, "pins_task", PINS_TASK_STACK, NULL, PINS_TASK_PRIORITY, &pins_task_h);
    bits_set(BIT_PINS_READY);
}

esp_err_t pins_config_in(
    gpio_num_t pin, bool pullup, bool pulldown, gpio_int_type_t intr, pins_callback_t callback) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = pulldown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = intr};
    gpio_config(&io_conf);
        if (callback) {
            pins_add_callback(pin, callback);
            gpio_isr_handler_add(pin, pins_isr_handler, (void *)pin);
    }
    return ESP_OK;
}

esp_err_t pins_config_out(gpio_num_t pin, bool pullup, bool pulldown, gpio_int_type_t intr) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = pulldown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
    return ESP_OK;
}

esp_err_t pins_write(gpio_num_t pin, int val) { return gpio_set_level((gpio_num_t)pin, val); }

int pins_read(gpio_num_t pin) { return gpio_get_level((gpio_num_t)pin); }
