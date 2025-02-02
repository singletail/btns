#include "ctrl_net.h"

esp_netif_t *s_sta_netif = NULL;

wifi_config_t wifi_config = {
    .sta = {.ssid = WIFI_SSID,
        .password = WIFI_PASS,
        .threshold.authmode = WIFI_AUTH_WPA_PSK},
};

static void ctrl_netif_init(void) {
    NOERR(esp_netif_init());
    NOERR(esp_event_loop_create_default());
}

static void ctrl_wifi_configure(void) {
    s_sta_netif = esp_netif_create_default_wifi_sta();
    NONULL(s_sta_netif);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    OK(esp_wifi_init(&cfg));
    net_evt_add_handlers(s_sta_netif);
    OK(esp_wifi_set_mode(WIFI_MODE_STA));
    OK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
}

static void ctrl_net_stack(void *arg) {
    INFO("ctrl_net_stack() started");
    int num_errors = 0;
    int loop_delay = 1000;

    while (1) {
        if (bits_chk(BIT_NET_OK) && bits_chk(BIT_TIME_OK) &&
            bits_chk(BIT_MQTT_OK) && bits_chk(BIT_HA_OK)) {
            bits_on(BIT_NET_STACK_OK);
            num_errors = 0;
            loop_delay = 10000;
            //printf("%s[%s] Network ok%s\n", col_g, sym_net, col_reset);
        } else {
            bits_off(BIT_NET_STACK_OK);
            printf("%s[%s] bits_chk(BIT_NET_OK) = %s%s\n", col_r, sym_net,
                bits_chk(BIT_NET_OK) ? "true" : "false", col_reset);
            printf("%s[%s] bits_chk(BIT_TIME_OK) = %s%s\n", col_o, sym_net,
                bits_chk(BIT_TIME_OK) ? "true" : "false", col_reset);
            printf("%s[%s] bits_chk(BIT_MQTT_OK) = %s%s\n", col_y, sym_net,
                bits_chk(BIT_MQTT_OK) ? "true" : "false", col_reset);
            printf("%s[%s] bits_chk(BIT_HA_OK) = %s%s\n", col_g, sym_net,
                bits_chk(BIT_HA_OK) ? "true" : "false", col_reset);
            printf("%s[%s] bits_chk(BIT_NET_STACK_OK) = %s%s\n", col_b, sym_net,
                bits_chk(BIT_NET_STACK_OK) ? "true" : "false", col_reset);

            num_errors++;
            if (num_errors > 40) {
                esp_restart();
            } else if (num_errors > 20) {
                loop_delay = (500 * num_errors);
            } else {
                loop_delay = 1000;
            }

            ha_reset_network();
            bits_off(BIT_HA_OK);

            printf("%s[%s] Network error %d. Retrying in %d seconds.%s\n",
                col_r, sym_net, num_errors, (loop_delay / 1000), col_reset);
        }

        if (bits_chk(BIT_NET_STACK_OK) == false) {
            if (bits_chk(BIT_NET_OK) == false) {
                ctrl_wifi_connect();
            } else if (bits_chk(BIT_TIME_OK) == false) {
                time_timex();
            } else if (!bits_chk(BIT_MQTT_OK)) {
                broker_init();
            } else if (!bits_chk(BIT_HA_OK)) {
                ha_init();
            }
        }
        vTaskDelay(loop_delay / portTICK_PERIOD_MS);
    }
}

static void ctrl_net_stack_start_task(void) {
    bits_wait(BIT_NET_OK);
    xTaskCreate(ctrl_net_stack, "ctrl_net_stack", 4096, NULL, 5, NULL);
}

void ctrl_wifi_connect(void) {
    if (bits_get(BIT_NET_FAIL))
        esp_wifi_disconnect();
    bits_wait(BIT_NET_INIT);
    OK(esp_wifi_connect());
    ctrl_net_stack_start_task();
}

void ctrl_net_init(void) {
    bits_wait(BIT_GPIO_OK);
    ctrl_netif_init();
    ctrl_wifi_configure();
    esp_wifi_start();
    ctrl_wifi_connect();
}
