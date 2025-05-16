#include "btns.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <string.h>

#define TAG "BTN"
#define BTN_MAX_COUNT 16
#define BTN_TASK_STACK_SIZE 2048
#define BTN_TASK_PRIORITY 5
#define BTN_SCAN_INTERVAL_MS 10
#define BTN_QUEUE_SIZE 16

// Button state structure
typedef struct btn_handle_s {
    btn_config_t config;       // Button configuration
    bool last_state;           // Last raw button state
    bool current_state;        // Current debounced state
    uint32_t last_change_time; // Time of last state change
    bool is_long_press;        // Flag for long press detection
    bool led_state;            // Current LED state
} btn_t;

// Button event
typedef struct {
    btn_t *button;
    btn_event_t event;
} btn_event_msg_t;

// Static variables
static QueueHandle_t btn_event_queue = NULL;
static TaskHandle_t btn_task_handle = NULL;
static SemaphoreHandle_t btn_mutex = NULL;
static btn_t *buttons[BTN_MAX_COUNT] = {0};
static int btn_count = 0;

// Function prototypes
static void btn_task(void *arg);
static bool btn_read_raw(btn_t *btn);

esp_err_t btn_init(void) {
    if (btn_event_queue) {
        return ESP_OK; // Already initialized
    }

    // Create event queue
    btn_event_queue = xQueueCreate(BTN_QUEUE_SIZE, sizeof(btn_event_msg_t));
    if (!btn_event_queue) {
        ESP_LOGE(TAG, "Failed to create button event queue");
        return ESP_FAIL;
    }

    // Create mutex
    btn_mutex = xSemaphoreCreateMutex();
    if (!btn_mutex) {
        vQueueDelete(btn_event_queue);
        btn_event_queue = NULL;
        ESP_LOGE(TAG, "Failed to create button mutex");
        return ESP_FAIL;
    }

    // Create button task
    BaseType_t ret = xTaskCreate(
        btn_task,
        "btn_task",
        BTN_TASK_STACK_SIZE,
        NULL,
        BTN_TASK_PRIORITY,
        &btn_task_handle
    );

    if (ret != pdPASS) {
        vSemaphoreDelete(btn_mutex);
        vQueueDelete(btn_event_queue);
        btn_mutex = NULL;
        btn_event_queue = NULL;
        ESP_LOGE(TAG, "Failed to create button task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Button module initialized");
    return ESP_OK;
}

esp_err_t btn_create(const btn_config_t *config, btn_handle_t *handle_out) {
    if (!config || !handle_out || !btn_event_queue) {
        return ESP_ERR_INVALID_ARG;
    }

    // Check if we have room for another button
    if (xSemaphoreTake(btn_mutex, portMAX_DELAY) != pdTRUE) {
        return ESP_FAIL;
    }

    if (btn_count >= BTN_MAX_COUNT) {
        xSemaphoreGive(btn_mutex);
        ESP_LOGE(TAG, "Maximum button count reached");
        return ESP_FAIL;
    }

    // Allocate button instance
    btn_t *btn = (btn_t *)calloc(1, sizeof(btn_t));
    if (!btn) {
        xSemaphoreGive(btn_mutex);
        ESP_LOGE(TAG, "Failed to allocate button");
        return ESP_ERR_NO_MEM;
    }

    // Copy configuration
    memcpy(&btn->config, config, sizeof(btn_config_t));

    // Configure button GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << config->pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = config->active_low ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = config->active_low ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        free(btn);
        xSemaphoreGive(btn_mutex);
        ESP_LOGE(TAG, "Failed to configure button GPIO");
        return ret;
    }

    // Configure LED GPIO if specified
    if (config->led_pin != GPIO_NUM_NC) {
        io_conf.pin_bit_mask = 1ULL << config->led_pin;
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        
        ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            free(btn);
            xSemaphoreGive(btn_mutex);
            ESP_LOGE(TAG, "Failed to configure LED GPIO");
            return ret;
        }
        
        // Initialize LED to off
        gpio_set_level(config->led_pin, 0);
    }

    // Initialize button state
    btn->last_state = btn_read_raw(btn);
    btn->current_state = btn->last_state;
    btn->last_change_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

    // Add button to array
    buttons[btn_count++] = btn;
    *handle_out = (btn_handle_t)btn;
    
    xSemaphoreGive(btn_mutex);
    ESP_LOGI(TAG, "Button created on GPIO %d", config->pin);
    return ESP_OK;
}

esp_err_t btn_delete(btn_handle_t handle) {
    if (!handle || !btn_event_queue) {
        return ESP_ERR_INVALID_ARG;
    }

    btn_t *btn = (btn_t *)handle;
    
    if (xSemaphoreTake(btn_mutex, portMAX_DELAY) != pdTRUE) {
        return ESP_FAIL;
    }

    // Find and remove button from array
    int index = -1;
    for (int i = 0; i < btn_count; i++) {
        if (buttons[i] == btn) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        // Shift remaining buttons
        for (int i = index; i < btn_count - 1; i++) {
            buttons[i] = buttons[i + 1];
        }
        btn_count--;
    }

    // Free button memory
    free(btn);
    
    xSemaphoreGive(btn_mutex);
    ESP_LOGI(TAG, "Button deleted");
    return ESP_OK;
}

esp_err_t btn_set_led(btn_handle_t handle, bool state) {
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    btn_t *btn = (btn_t *)handle;
    
    if (btn->config.led_pin == GPIO_NUM_NC) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    btn->led_state = state;
    gpio_set_level(btn->config.led_pin, state ? 1 : 0);
    return ESP_OK;
}

bool btn_is_pressed(btn_handle_t handle) {
    if (!handle) {
        return false;
    }
    
    btn_t *btn = (btn_t *)handle;
    return btn->current_state;
}

// Read raw button state accounting for active low/high
static bool btn_read_raw(btn_t *btn) {
    int level = gpio_get_level(btn->config.pin);
    return btn->config.active_low ? !level : level;
}

// Button task - handles debouncing and event generation
static void btn_task(void *arg) {
    TickType_t last_wake_time = xTaskGetTickCount();
    btn_event_msg_t event_msg;

    while (1) {
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        if (xSemaphoreTake(btn_mutex, portMAX_DELAY) == pdTRUE) {
            // Process each button
            for (int i = 0; i < btn_count; i++) {
                btn_t *btn = buttons[i];
                if (!btn) continue;

                // Read current button state
                bool raw_state = btn_read_raw(btn);
                
                // Handle debouncing
                if (raw_state != btn->last_state) {
                    btn->last_change_time = now;
                    btn->last_state = raw_state;
                } else if (raw_state != btn->current_state && 
                           now - btn->last_change_time >= btn->config.debounce_ms) {
                    // State change confirmed
                    btn->current_state = raw_state;
                    
                    if (btn->current_state) {
                        // Button pressed
                        if (btn->config.callback) {
                            event_msg.button = btn;
                            event_msg.event = BTN_EVENT_PRESSED;
                            xQueueSend(btn_event_queue, &event_msg, 0);
                        }
                    } else {
                        // Button released
                        if (btn->config.callback) {
                            event_msg.button = btn;
                            event_msg.event = BTN_EVENT_RELEASED;
                            xQueueSend(btn_event_queue, &event_msg, 0);
                        }
                        
                        // Handle click or end of long press
                        if (btn->is_long_press) {
                            btn->is_long_press = false;
                        } else if (btn->config.callback) {
                            event_msg.button = btn;
                            event_msg.event = BTN_EVENT_CLICKED;
                            xQueueSend(btn_event_queue, &event_msg, 0);
                        }
                    }
                }
                
                // Check for long press
                if (btn->current_state && !btn->is_long_press &&
                    now - btn->last_change_time >= btn->config.long_press_ms) {
                    btn->is_long_press = true;
                    if (btn->config.callback) {
                        event_msg.button = btn;
                        event_msg.event = BTN_EVENT_LONG_PRESSED;
                        xQueueSend(btn_event_queue, &event_msg, 0);
                    }
                }
            }
            
            xSemaphoreGive(btn_mutex);
        }

        // Process button events
        while (xQueueReceive(btn_event_queue, &event_msg, 0) == pdTRUE) {
            btn_t *btn = event_msg.button;
            if (btn->config.callback) {
                btn->config.callback(btn->config.user_data, event_msg.event);
            }
        }
        
        // Run at fixed interval for consistent debouncing
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(BTN_SCAN_INTERVAL_MS));
    }
}

