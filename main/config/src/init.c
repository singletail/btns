#include "init.h"
#include "leds.h"
#include "btns.h"

// Declare the button handler and task function
void led_mode_btn_handler(void *arg, btn_event_t event);
void multi_mode_led_task(void *arg);

void btn_test(void *arg, btn_event_t event) {
    int btn_id = (int)arg;
    
    switch (event) {
        case BTN_EVENT_PRESSED:
            printf("Button %d pressed\n", btn_id);
            break;
        case BTN_EVENT_RELEASED:
            printf("Button %d released\n", btn_id);
            break;
        case BTN_EVENT_CLICKED:
            printf("Button %d clicked\n", btn_id);
            break;
        case BTN_EVENT_LONG_PRESSED:
            printf("Button %d long pressed\n", btn_id);
            break;
    }
}

void init() {
    INFO("init()");
    //xTaskCreate(maybe_task, "maybe", 8192, NULL, 2, NULL);
    xTaskCreate(quarterflash_task, "quarterflash", 8192, NULL, 2, NULL);

/*
    // Initialize button module
    btn_init();
    
    // Configure button for LED mode switching
    btn_config_t btn1_config = {
        .pin = GPIO_NUM_7,
        .led_pin = GPIO_NUM_NC,
        .debounce_ms = 50,
        .long_press_ms = 1000,
        .active_low = true,
        .callback = led_mode_btn_handler,
        .user_data = (void*)1
    };

    btn_handle_t btn1_handle;
    btn_create(&btn1_config, &btn1_handle);
    
    // Create the multi-mode LED task
    xTaskCreate(multi_mode_led_task, "multi_led", 8192, NULL, 2, NULL);
*/

    // Create wargames task
    xTaskCreate(wargames_task, "wargames", 2048, NULL, 2, NULL);


    // audio test
    xTaskCreate(pcm5102_test_task, "pcm5102_test", 4096, NULL, 5, NULL);

    
    INFO("Done.");
}