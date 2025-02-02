#include "pin.h"

#define PIN_QUEUE_SIZE 10
#define PIN_TASK_STACK 4096
#define PIN_TASK_PRIORITY 1
#define PIN_DEBOUNCE_MS 100

static QueueHandle_t pin_queue_h = NULL;
static TaskHandle_t pin_task_h = NULL;
static pin_obj_t pins[SOC_GPIO_PIN_COUNT] = {0};

static void IRAM_ATTR pin_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(pin_queue_h, &gpio_num, NULL);
}

static void pin_fire_callback(uint32_t pin) {
    pin_obj_t *obj = &pins[pin];
    if (obj->callback) {
        obj->callback(pin);
    }
    // pins[pin].callback(pin, pins[pin].val);
}

static void pin_task(void *arg) {
    uint32_t pin;
    while (1) {
        if (xQueueReceive(pin_queue_h, &pin, portMAX_DELAY)) {
            // printf("pin_task() pin %d\n", pin);
            pin_fire_callback(pin);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static esp_err_t pin_add_callback(gpio_num_t pin, pin_callback_t callback) {
    pin_obj_t *pin_obj = &pins[pin];
    memset(pin_obj, 0, sizeof(pin_obj_t));
    pin_obj->pin = pin;
    pin_obj->val = 0;
    pin_obj->last = 0;
    pin_obj->callback = callback;
    return ESP_OK;
}

void pin_init(void) {
    LOG("pin_init()");
    pin_queue_h = xQueueCreate(PIN_QUEUE_SIZE, sizeof(int));
    FAIL(gpio_install_isr_service(ESP_INTR_FLAG_IRAM));
    xTaskCreate(pin_task, "pin_task", PIN_TASK_STACK, NULL, PIN_TASK_PRIORITY, &pin_task_h);
    bits_on(BIT_GPIO_OK);
}
/*
bool pin_configure(uint64_t mask, gpio_mode_t mode, gpio_pullup_t pullup,
    gpio_pulldown_t pulldown, gpio_int_type_t intr) {
    gpio_config_t io_conf = {.pin_bit_mask = mask,
        .mode = mode,
        .pull_up_en = pullup,
        .pull_down_en = pulldown,
        .intr_type = intr};
    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK)
        WARN("pin_configure() %d failed: %s.", mask, esp_err_to_name(err));
    return err == ESP_OK;
}
*/



esp_err_t pin_config_in(gpio_num_t pin, bool pullup, bool pulldown,
    gpio_int_type_t intr, pin_callback_t callback) {
    gpio_config_t io_conf = {.pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = pulldown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = intr};
    NOERR(gpio_config(&io_conf));
    if (callback) {
        NOERR(pin_add_callback(pin, callback));
        gpio_isr_handler_add(pin, pin_isr_handler, (void *) pin);
    }
    return ESP_OK;
}

esp_err_t pin_config_out(gpio_num_t pin, bool pullup, bool pulldown,
    gpio_int_type_t intr){
    gpio_config_t io_conf = {.pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = pulldown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    NOERR(gpio_config(&io_conf));
    return ESP_OK;
}

esp_err_t pin_write(gpio_num_t pin, int val) {
    return gpio_set_level((gpio_num_t)pin, val);
}

int pin_read(gpio_num_t pin) {
    return gpio_get_level((gpio_num_t)pin);
}
