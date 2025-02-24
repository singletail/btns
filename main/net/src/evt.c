#include "evt.h"

esp_event_handler_instance_t instance_any_id;
esp_event_handler_instance_t instance_got_ip;

static const char *wifi_event_names[] = {
    [WIFI_EVENT_WIFI_READY] = "WIFI_READY",
    [WIFI_EVENT_SCAN_DONE] = "SCAN_DONE",
    [WIFI_EVENT_STA_START] = "STA_START",
    [WIFI_EVENT_STA_STOP] = "STA_STOP",
    [WIFI_EVENT_STA_CONNECTED] = "STA_CONNECTED",
    [WIFI_EVENT_STA_DISCONNECTED] = "STA_DISCONNECTED",
    [WIFI_EVENT_STA_AUTHMODE_CHANGE] = "STA_AUTHMODE_CHANGE",
    [WIFI_EVENT_STA_WPS_ER_SUCCESS] = "STA_WPS_ER_SUCCESS",
    [WIFI_EVENT_STA_WPS_ER_FAILED] = "STA_WPS_ER_FAILED",
    [WIFI_EVENT_STA_WPS_ER_TIMEOUT] = "STA_WPS_ER_TIMEOUT",
    [WIFI_EVENT_STA_WPS_ER_PIN] = "STA_WPS_ER_PIN",
    [WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP] = "STA_WPS_ER_PBC_OVERLAP",
    [WIFI_EVENT_AP_START] = "AP_START",
    [WIFI_EVENT_AP_STOP] = "AP_STOP",
    [WIFI_EVENT_AP_STACONNECTED] = "AP_STACONNECTED",
    [WIFI_EVENT_AP_STADISCONNECTED] = "AP_STADISCONNECTED",
    [WIFI_EVENT_AP_PROBEREQRECVED] = "AP_PROBEREQRECVED",
    [WIFI_EVENT_FTM_REPORT] = "FTM_REPORT",
    [WIFI_EVENT_STA_BSS_RSSI_LOW] = "STA_BSS_RSSI_LOW",
    [WIFI_EVENT_ACTION_TX_STATUS] = "ACTION_TX_STATUS",
    [WIFI_EVENT_ROC_DONE] = "ROC_DONE",
    [WIFI_EVENT_STA_BEACON_TIMEOUT] = "STA_BEACON_TIMEOUT",
    [WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START] = "CONNECTIONLESS_WAKE_START",
    [WIFI_EVENT_AP_WPS_RG_SUCCESS] = "AP_WPS_RG_SUCCESS",
    [WIFI_EVENT_AP_WPS_RG_FAILED] = "AP_WPS_RG_FAILED",
    [WIFI_EVENT_AP_WPS_RG_TIMEOUT] = "AP_WPS_RG_TIMEOUT",
    [WIFI_EVENT_AP_WPS_RG_PIN] = "AP_WPS_RG_PIN",
    [WIFI_EVENT_AP_WPS_RG_PBC_OVERLAP] = "AP_WPS_RG_PBC_OVERLAP",
    [WIFI_EVENT_ITWT_SETUP] = "ITWT_SETUP",
    [WIFI_EVENT_ITWT_TEARDOWN] = "ITWT_TEARDOWN",
    [WIFI_EVENT_ITWT_PROBE] = "ITWT_PROBE",
    [WIFI_EVENT_ITWT_SUSPEND] = "ITWT_SUSPEND",
    [WIFI_EVENT_TWT_WAKEUP] = "TWT_WAKEUP",
    [WIFI_EVENT_BTWT_SETUP] = "BTWT_SETUP",
    [WIFI_EVENT_BTWT_TEARDOWN] = "BTWT_TEARDOWN",
    [WIFI_EVENT_NAN_STARTED] = "NAN_STARTED",
    [WIFI_EVENT_NAN_STOPPED] = "NAN_STOPPED",
    [WIFI_EVENT_NAN_SVC_MATCH] = "NAN_SVC_MATCH",
    [WIFI_EVENT_NAN_REPLIED] = "NAN_REPLIED",
    [WIFI_EVENT_NAN_RECEIVE] = "NAN_RECEIVE",
    [WIFI_EVENT_NDP_INDICATION] = "NDP_INDICATION",
    [WIFI_EVENT_NDP_CONFIRM] = "NDP_CONFIRM",
    [WIFI_EVENT_NDP_TERMINATED] = "NDP_TERMINATED",
    [WIFI_EVENT_HOME_CHANNEL_CHANGE] = "HOME_CHANNEL_CHANGE",
    [WIFI_EVENT_STA_NEIGHBOR_REP] = "STA_NEIGHBOR_REP",
    [WIFI_EVENT_MAX] = "INVALID_EVENT"};

static const char *wifi_event_name(wifi_event_t event) {
        if (event >= WIFI_EVENT_MAX) {
            return "UNKNOWN_EVENT";
    }
    return wifi_event_names[event] ? wifi_event_names[event] : "UNNAMED_EVENT";
}

static void evt_wifi_unhandled(esp_event_base_t evt_base, int32_t evt_id, void *evt_data) {
    WARN(
        "Unhandled Wi-Fi event", wifi_event_name(evt_id), evt_id);
}

static void evt_wifi_error(void *evt_data) {
    bits_clr(BIT_NET_READY);
    bits_set(BIT_NET_FAIL);
    wifi_event_sta_disconnected_t *evt = (wifi_event_sta_disconnected_t *)evt_data;
    WARN("WiFi Disconnect", evt->reason);
}

void evt_wifi(void *arg, esp_event_base_t evt_base, int32_t evt_id, void *evt_data) {
        switch (evt_id) {
            case WIFI_EVENT_STA_START:
                bits_set(BIT_NET_INIT);
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                evt_wifi_error(evt_data);
                break;
            default:
                evt_wifi_unhandled(evt_base, evt_id, evt_data);
                break;
        }
}

void evt_wifi_ip(void *arg, esp_event_base_t evt_base, int32_t evt_id, void *event_data) {
        switch (evt_id) {
            case IP_EVENT_STA_GOT_IP:
                printf("%s[%s] Connected with IP. %s\n", col_p, sym_net, col_reset);
                bits_set(BIT_NET_READY);
                bits_clr(BIT_NET_FAIL);
                break;
            default:
                break;}
    return;
}

void evt_add_handlers(esp_netif_t *s_sta_netif) {
    esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &evt_wifi, s_sta_netif, &instance_any_id);
    esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &evt_wifi_ip, s_sta_netif, &instance_got_ip);
}
