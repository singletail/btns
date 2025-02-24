#include "net.h"

esp_netif_t *s_sta_netif = NULL;

wifi_config_t wifi_config = {
    .sta = {.ssid = WIFI_SSID, .password = WIFI_PASS, .threshold.authmode = WIFI_AUTH_WPA_PSK},
};

static void net_init_netif(void) {
    esp_netif_init();
    esp_event_loop_create_default();
}

static void net_wifi_configure(void) {
    s_sta_netif = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    evt_add_handlers(s_sta_netif);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
}

static void net_stack_task(void *arg) {
    INFO("net_stack_task() started");
    int num_errors = 0;
    int loop_delay = 1000;
        while (1) {
            loop_delay = (1000 * (num_errors + 1));
            if (bits_bool(BIT_NET_READY) == false) {
                    num_errors++;
                    bits_set(BIT_NET_FAIL);
                    bits_clr(BIT_HA_READY);
                    net_wifi_connect();
                } else if (bits_bool(BIT_NTP_READY) == false) {
                    num_errors++;
                    bits_clr(BIT_NTP_READY);
                    ntp_init();
                } else if (!bits_bool(BIT_MQTT_READY)) {
                    num_errors++;
                    bits_clr(BIT_MQTT_READY);
                    bits_clr(BIT_HA_READY);
                    mqtt_init();
                } else if (!bits_bool(BIT_HA_READY)) {
                    num_errors++;
                    ha_init();
                } else {
                    bits_set(BIT_NET_OK);
                    num_errors = 0;
                    loop_delay = 10000;
                }
                if (num_errors > 0) {
                    bits_clr(BIT_NET_OK);
                    printf("%s%s %s%s %s%s %s%s %s\n", bits_bool(BIT_NET_READY)?col_g :col_r, sym_net, bits_bool(BIT_NTP_READY)?col_g :col_r, sym_clock, bits_bool(BIT_MQTT_READY)?col_g :col_r, sym_mqtt, bits_bool(BIT_HA_READY)?col_g :col_r, sym_ha, col_reset);
                }
                vTaskDelay(loop_delay / portTICK_PERIOD_MS);
        }
}

static void net_stack_start(void) {
    bits_wait(BIT_NET_READY);
    xTaskCreate(net_stack_task, "net_stack_task", 4096, NULL, 5, NULL);
}

void net_wifi_connect(void) {
    if (bits_bool(BIT_NET_FAIL)) esp_wifi_disconnect();
    bits_wait(BIT_NET_INIT);
    esp_wifi_connect();
    net_stack_start();
}

void net_init(void) {
    bits_wait(BIT_PINS_READY);
    net_init_netif();
    net_wifi_configure();
    esp_wifi_start();
    net_wifi_connect();
}
