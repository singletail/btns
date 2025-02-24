#include "ha.h"

char ha_buffer[2048];

static bool status_discovery_published = false;
static bool status_availability_published = false;
static bool status_state_published = false;

const char *HA_TOPIC_AVAIL = MQTT_TOPIC "/device/" DEVICE_ID "/status";
const char *HA_TOPIC_STATE = MQTT_TOPIC "/device/" DEVICE_ID "/state";
const char *HA_TOPIC_DISCOVERY = MQTT_TOPIC "/device/" DEVICE_ID "/config";
const char *HA_TOPIC_CMD = MQTT_TOPIC "/device/" DEVICE_ID "/cmd";

const char *HA_MSG_AVAIL_ONLINE = "online";
const char *HA_MSG_AVAIL_OFFLINE = "offline";

static inline char *sensor_value_to_string(int sensor_id) {
    static char value_str[32];
    value_t value = sensor_get_value(sensor_id);

        switch (sensor_get_type(sensor_id)) {
            case VAL_TYPE_BOOL:
                snprintf(value_str, sizeof(value_str), "%s", value.b ? "true" : "false");
                break;
            case VAL_TYPE_INT:
                snprintf(value_str, sizeof(value_str), "%d", value.i);
                break;
            case VAL_TYPE_FLOAT:
                snprintf(value_str, sizeof(value_str), "%.2f", value.f);
                break;
            case VAL_TYPE_STRING:
                snprintf(value_str, sizeof(value_str), "\"%s\"", value.s);
                break;
            default:
                value_str[0] = '\0';
        }
    return value_str;
}

void ha_init(void) {
    LOG("ha_init()");
    bits_wait(BIT_MQTT_READY);

    if (!status_discovery_published) status_discovery_published = ha_publish_discovery();

    if (!status_availability_published) status_availability_published = ha_publish_availability();

    if (!status_state_published) status_state_published = ha_publish_state();

        if (status_discovery_published && status_availability_published && status_state_published) {
            printf("%s[%s] HA initialized%s\n", col_g, sym_ha, col_reset);
            bits_set(BIT_HA_READY);
    }
}

void ha_reset_network(void) {
    status_discovery_published = false;
    status_availability_published = false;
    status_state_published = false;
}

bool ha_publish_availability(void) {
    INFO("ha_publish_availability()");
    printf("%s[%s] Publishing availability: online%s\n", col_c, sym_ha, col_reset);
    return mqtt_publish(HA_TOPIC_AVAIL, "online", 0, 1) > -1;
}

bool ha_publish_state(void) {
    INFO("ha_publish_state()");
        if (bits_bool(BIT_MQTT_READY) == false) {
            return false;
    }

    ha_buffer[0] = '\0';
    strcpy(ha_buffer, "{");

    int num_sensors = sensor_get_count();
        for (int i = 0; i < num_sensors; i++) {
            sensor_info_t info = sensor_get_info(i);

            // Add quotes around key
            strcpy(ha_buffer + strlen(ha_buffer), "\"");
            strcpy(ha_buffer + strlen(ha_buffer), info.unique_id);
            strcpy(ha_buffer + strlen(ha_buffer), "\":");

            // Add value (with or without quotes depending on type)
            char *value = sensor_value_to_string(i);
            strcpy(ha_buffer + strlen(ha_buffer), value);

                // Add comma if not last
                if (i < num_sensors - 1) {
                    strcpy(ha_buffer + strlen(ha_buffer), ",");
            }
        }
    strcpy(ha_buffer + strlen(ha_buffer), "}");

    printf("%s[%s] Publishing state: %s%s\n", col_c, sym_ha, ha_buffer, col_reset);

    return mqtt_publish(HA_TOPIC_STATE, ha_buffer, 0, 0) > -1;
}

bool ha_publish_discovery(void) {
    INFO("ha_publish_discovery()");
    ha_buffer[0] = '\0';
    strcpy(ha_buffer + strlen(ha_buffer), "{");

    strcpy(ha_buffer + strlen(ha_buffer), "\"device\": {");

    strcpy(ha_buffer + strlen(ha_buffer), "\"name\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_NAME);
    strcpy(ha_buffer + strlen(ha_buffer), "\",");

    strcpy(ha_buffer + strlen(ha_buffer), "\"identifiers\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_ID);
    strcpy(ha_buffer + strlen(ha_buffer), "\",");

    strcpy(ha_buffer + strlen(ha_buffer), "\"model\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_MODEL);
    strcpy(ha_buffer + strlen(ha_buffer), "\",");

    strcpy(ha_buffer + strlen(ha_buffer), "\"manufacturer\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_MANUFACTURER);
    strcpy(ha_buffer + strlen(ha_buffer), "\",");

    strcpy(ha_buffer + strlen(ha_buffer), "\"sw_version\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_SW_VERSION);
    strcpy(ha_buffer + strlen(ha_buffer), "\",");

    strcpy(ha_buffer + strlen(ha_buffer), "\"hw_version\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_HW_VERSION);
    strcpy(ha_buffer + strlen(ha_buffer), "\"");

    strcpy(ha_buffer + strlen(ha_buffer), "},");

    strcpy(ha_buffer + strlen(ha_buffer), "\"origin\": {");

    strcpy(ha_buffer + strlen(ha_buffer), "\"name\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_ID);
    strcpy(ha_buffer + strlen(ha_buffer), "\",");

    strcpy(ha_buffer + strlen(ha_buffer), "\"sw_version\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_SW_VERSION);
    strcpy(ha_buffer + strlen(ha_buffer), "\"");

    strcpy(ha_buffer + strlen(ha_buffer), "},");

    int num_sensors = sensor_get_count();
        if (num_sensors > 0) {
            strcpy(ha_buffer + strlen(ha_buffer), "\"components\": {");
                for (int i = 0; i < num_sensors; i++) {
                    sensor_info_t info = sensor_get_info(i);

                    strcpy(ha_buffer + strlen(ha_buffer), "\"");
                    strcpy(ha_buffer + strlen(ha_buffer), info.name);
                    strcpy(ha_buffer + strlen(ha_buffer), "\": {");

                    strcpy(ha_buffer + strlen(ha_buffer), "\"platform\": \"");
                    strcpy(ha_buffer + strlen(ha_buffer), info.component_type);
                    strcpy(ha_buffer + strlen(ha_buffer), "\",");

                    strcpy(ha_buffer + strlen(ha_buffer), "\"device_class\": \"");
                    strcpy(ha_buffer + strlen(ha_buffer), info.device_class);
                    strcpy(ha_buffer + strlen(ha_buffer), "\",");

                    strcpy(ha_buffer + strlen(ha_buffer), "\"unique_id\": \"");
                    strcpy(ha_buffer + strlen(ha_buffer), DEVICE_ID);
                    strcpy(ha_buffer + strlen(ha_buffer), "_");
                    strcpy(ha_buffer + strlen(ha_buffer), info.unique_id);
                    strcpy(ha_buffer + strlen(ha_buffer), "\",");

                    strcpy(ha_buffer + strlen(ha_buffer), "\"value_template\": \"{{ value_json.");
                    strcpy(ha_buffer + strlen(ha_buffer), info.unique_id);
                    strcpy(ha_buffer + strlen(ha_buffer), " }}\",");

                    strcpy(ha_buffer + strlen(ha_buffer), "\"payload_available\": \"online\",");
                    strcpy(
                        ha_buffer + strlen(ha_buffer), "\"payload_not_available\": \"offline\",");

                        if (info.unit) {
                            strcpy(ha_buffer + strlen(ha_buffer), "\"unit_of_measurement\": \"");
                            strcpy(ha_buffer + strlen(ha_buffer), info.unit);
                            strcpy(ha_buffer + strlen(ha_buffer), "\",");
                    }

                        if (info.icon) {
                            strcpy(ha_buffer + strlen(ha_buffer), "\"icon\": \"");
                            strcpy(ha_buffer + strlen(ha_buffer), info.icon);
                            strcpy(ha_buffer + strlen(ha_buffer), "\",");
                    }

                        if (info.payload_on) {
                            strcpy(ha_buffer + strlen(ha_buffer), "\"payload_on\": \"");
                            strcpy(ha_buffer + strlen(ha_buffer), info.payload_on);
                            strcpy(ha_buffer + strlen(ha_buffer), "\",");
                    }

                        if (info.payload_off) {
                            strcpy(ha_buffer + strlen(ha_buffer), "\"payload_off\": \"");
                            strcpy(ha_buffer + strlen(ha_buffer), info.payload_off);
                            strcpy(ha_buffer + strlen(ha_buffer), "\",");
                    }

                    strcpy(ha_buffer + strlen(ha_buffer), "\"availability_topic\": \"");
                    strcpy(ha_buffer + strlen(ha_buffer), HA_TOPIC_AVAIL);
                    strcpy(ha_buffer + strlen(ha_buffer), "\",");

                    strcpy(ha_buffer + strlen(ha_buffer), "\"availability_mode\": \"all\",");

                    strcpy(ha_buffer + strlen(ha_buffer), "\"state_topic\": \"");
                    strcpy(ha_buffer + strlen(ha_buffer), HA_TOPIC_STATE);
                    strcpy(ha_buffer + strlen(ha_buffer), "\",");

                    strcpy(ha_buffer + strlen(ha_buffer), "\"command_topic\": \"");
                    strcpy(ha_buffer + strlen(ha_buffer), HA_TOPIC_CMD);
                    strcpy(ha_buffer + strlen(ha_buffer), "\"");

                    strcpy(ha_buffer + strlen(ha_buffer), "}");
                        if (i < num_sensors - 1) {
                            strcpy(ha_buffer + strlen(ha_buffer), ",");
                    }
                }
            strcpy(ha_buffer + strlen(ha_buffer), "},");
    }
    strcpy(ha_buffer + strlen(ha_buffer), "\"state_topic\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), HA_TOPIC_STATE);
    strcpy(ha_buffer + strlen(ha_buffer), "\",");

    strcpy(ha_buffer + strlen(ha_buffer), "\"command_topic\": \"");
    strcpy(ha_buffer + strlen(ha_buffer), HA_TOPIC_CMD);
    strcpy(ha_buffer + strlen(ha_buffer), "\"");

    strcpy(ha_buffer + strlen(ha_buffer), "}");

    printf(
        "%s[%s] Publishing discovery. Topic: %s%s\n", col_r, sym_ha, HA_TOPIC_DISCOVERY, col_reset);
    printf("%s[%s] %s%s\n", col_c, sym_ha, ha_buffer, col_reset);

    int msg_id = mqtt_publish(HA_TOPIC_DISCOVERY, ha_buffer, 0, 0);
    return (msg_id > -1);
}
