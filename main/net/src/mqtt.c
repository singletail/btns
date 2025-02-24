#include "mqtt.h"

static esp_mqtt_client_handle_t mqtt_client;

static const char *topic_status = MQTT_TOPIC "/" DEVICE_ID "/status";
static const char *topic_subscribe = MQTT_TOPIC "/" DEVICE_ID "/cmd/#";

void mqtt_status(esp_err_t status) {
    (status == ESP_OK) ? bits_set(BIT_MQTT_READY) : bits_clr(BIT_MQTT_READY);
}

esp_err_t chk(int msg_id) {
    esp_err_t err_rc_;
    (msg_id > -1) ? err_rc_ = ESP_OK : ESP_FAIL;
    mqtt_status(err_rc_);
    return err_rc_;
}

static const esp_mqtt_client_config_t mqtt_config = {
    .broker.address.uri = MQTT_URI,
    .credentials.client_id = DEVICE_ID,
    .credentials.username = MQTT_USER,
    .credentials.authentication.password = MQTT_PASS,
    .network = {
            .reconnect_timeout_ms = 10000,
            .timeout_ms = 10000,
            .disable_auto_reconnect = true, },
    .session = {
        .keepalive = 30,
        .disable_clean_session = false,
        .last_will.topic = DEVICE_ID "/status",
        .last_will.msg = "offline",
        .last_will.qos = 1,
        .last_will.retain = 1,
    }};

static void mqtt_event_handler(
    void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
        switch ((esp_mqtt_event_id_t)event_id) {
            case MQTT_EVENT_CONNECTED:
                mqtt_connected();
                break;
            case MQTT_EVENT_DISCONNECTED:
                ERR("MQTT Disconnected");
                mqtt_status(ESP_FAIL);
                break;
            case MQTT_EVENT_SUBSCRIBED:
                break;
            case MQTT_EVENT_UNSUBSCRIBED:
                break;
            case MQTT_EVENT_PUBLISHED:
                break;
            case MQTT_EVENT_BEFORE_CONNECT:
                break;
            case MQTT_EVENT_DATA:
                mqtt_event_data(event);
                break;
            case MQTT_EVENT_ERROR:
                mqtt_error(event);
                break;
            default:
                WARN("Unhandled MQTT event", event->event_id);
        }
}

void mqtt_init(void) {
    DEBUG("mqtt_init()");
    bits_wait(BIT_NET_READY);
        if (mqtt_client != NULL) {
            esp_mqtt_client_start(mqtt_client);
            return;
    }
    mqtt_client = esp_mqtt_client_init(&mqtt_config);
        if (mqtt_client == NULL) {
            WARN("esp_mqtt_client_init() failed");
            return;
    }
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void mqtt_connected(void) {
    LOG("mqtt connected to server.");
    chk(esp_mqtt_client_publish(mqtt_client, topic_status, "online", 0, 0, 1));
    chk(esp_mqtt_client_subscribe_single(mqtt_client, topic_subscribe, 0));
}

int mqtt_publish(const char *topic, const char *payload, int qos, int retain) {
    bits_wait(BIT_MQTT_READY);
    return chk(esp_mqtt_client_publish(mqtt_client, topic, payload, 0, qos, retain));
}

void mqtt_event_data(esp_mqtt_event_t *event) {
    char *topic = event->topic;
    char *data = event->data;
    bool retain = event->retain;
    INFO("Received:\n", topic, data, retain ? "retain" : "no retain");
    // TODO: handle data
}

void mqtt_error(esp_mqtt_event_t *event) {
    esp_mqtt_error_codes_t *error_handle = event->error_handle;
    esp_err_t esp_tls_last_esp_err = error_handle->esp_tls_last_esp_err;
        if (esp_tls_last_esp_err != ESP_OK) {
            ERR("reported from esp-tls", esp_tls_last_esp_err);
    }
    int esp_tls_stack_err = error_handle->esp_tls_stack_err;
        if (esp_tls_stack_err != 0) {
            ERR("reported from tls stack", esp_tls_stack_err);
    }
    int esp_tls_cert_verify_flags = error_handle->esp_tls_cert_verify_flags;
        if (esp_tls_cert_verify_flags != 0) {
            ERR("reported from tls stack during certificate verification",
                esp_tls_cert_verify_flags);
    }
    esp_mqtt_error_type_t error_type = error_handle->error_type;
        if (error_type > 0) {
                switch (error_type) {
                    case MQTT_ERROR_TYPE_TCP_TRANSPORT:
                        ERR("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                        ERR("reported from tls stack", event->error_handle->esp_tls_stack_err);
                        ERR("captured as transport's socket errno",
                            event->error_handle->esp_transport_sock_errno);
                        break;
                    case MQTT_ERROR_TYPE_CONNECTION_REFUSED:
                        WARN("MQTT connection refused");
                        break;
                    case MQTT_ERROR_TYPE_SUBSCRIBE_FAILED:
                        WARN("MQTT subscribe failed");
                        break;
                    default:
                        WARN("Unhandled MQTT error", error_type);
                        break;
                }
    }
    esp_mqtt_connect_return_code_t connect_return_code = error_handle->connect_return_code;
        if (connect_return_code > 0) {
            WARN("MQTT connection refused");
    }
    int esp_transport_sock_errno = error_handle->esp_transport_sock_errno;
        if (esp_transport_sock_errno != 0) {
            ERR("captured as transport's socket errno", esp_transport_sock_errno);
    }
    mqtt_status(ESP_FAIL);
}

/*
void mqtt_subscribe(const char *topic) {
    xEventGroupWaitBits(ctrl_bits, BIT_MQTT_OK, pdFALSE, pdTRUE, portMAX_DELAY);
    INFO("mqtt_subscribe()");
    int msg_id = esp_mqtt_client_subscribe_single(mqtt_client, topic, 0);
    if (msg_id < 0) {
        WARN("esp_mqtt_client_subscribe_single() failed");
        return;
    }
    INFO("Subscribe success.", msg_id, topic);
}
*/


