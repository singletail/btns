#include "ha.h"

// #ifdef ENABLE_HA

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
        snprintf(
            value_str, sizeof(value_str), "%s", value.b ? "true" : "false");
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
    bits_wait(BIT_MQTT_OK);

    if (!status_discovery_published)
        status_discovery_published = ha_publish_discovery();

    if (!status_availability_published)
        status_availability_published = ha_publish_availability();

    if (!status_state_published)
        status_state_published = ha_publish_state();

    if (status_discovery_published && status_availability_published &&
        status_state_published) {
        printf("%s[%s] HA initialized%s\n", col_g, sym_ha, col_reset);
        bits_on(BIT_HA_OK);
    }
}

void ha_reset_network(void) {
    status_discovery_published = false;
    status_availability_published = false;
    status_state_published = false;
}

bool ha_publish_availability(void) {
    printf(
        "%s[%s] Publishing availability: online%s\n", col_c, sym_ha, col_reset);
    return broker_publish(HA_TOPIC_AVAIL, "online", 0, 1) > -1;
}

bool ha_publish_state(void) {
    if (bits_chk(BIT_MQTT_OK) == false) {
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

    printf(
        "%s[%s] Publishing state: %s%s\n", col_c, sym_ha, ha_buffer, col_reset);

    return broker_publish(HA_TOPIC_STATE, ha_buffer, 0, 0) > -1;
}

bool ha_publish_discovery(void) {
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

            strcpy(ha_buffer + strlen(ha_buffer),
                "\"value_template\": \"{{ value_json.");
            strcpy(ha_buffer + strlen(ha_buffer), info.unique_id);
            strcpy(ha_buffer + strlen(ha_buffer), " }}\",");

            strcpy(ha_buffer + strlen(ha_buffer),
                "\"payload_available\": \"online\",");
            strcpy(ha_buffer + strlen(ha_buffer),
                "\"payload_not_available\": \"offline\",");

            if (info.unit) {
                strcpy(ha_buffer + strlen(ha_buffer),
                    "\"unit_of_measurement\": \"");
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

            strcpy(ha_buffer + strlen(ha_buffer),
                "\"availability_mode\": \"all\",");

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

    printf("%s[%s] Publishing discovery. Topic: %s%s\n", col_r, sym_ha,
        HA_TOPIC_DISCOVERY, col_reset);
    printf("%s[%s] %s%s\n", col_c, sym_ha, ha_buffer, col_reset);

    int msg_id = broker_publish(HA_TOPIC_DISCOVERY, ha_buffer, 0, 0);
    return (msg_id > -1);
}

/*
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        ERR("Failed to create JSON object");
        return false;
    }

    // Device info
    cJSON *device = cJSON_CreateObject();
    if (!device) {
        ERR("Failed to create device object");
        cJSON_Delete(root);
        return false;
    }

    cJSON_AddStringToObject(device, "identifiers", DEVICE_ID);
    cJSON_AddStringToObject(device, "name", DEVICE_NAME);
    cJSON_AddStringToObject(device, "model", DEVICE_MODEL);
    cJSON_AddStringToObject(device, "manufacturer", DEVICE_MANUFACTURER);
    cJSON_AddStringToObject(device, "sw_version", DEVICE_SW_VERSION);
    cJSON_AddStringToObject(device, "hw_version", DEVICE_HW_VERSION);
    cJSON_AddItemToObject(root, "device", device);

    // Origin info
    cJSON *origin = cJSON_CreateObject();
    if (!origin) {
        ERR("Failed to create origin object");
        cJSON_Delete(root);
        return false;
    }

    cJSON_AddStringToObject(origin, "name", DEVICE_ID);
    cJSON_AddStringToObject(origin, "sw_version", DEVICE_SW_VERSION);
    cJSON_AddItemToObject(root, "origin", origin);

    // Components
    cJSON *components = cJSON_CreateObject();
    if (!components) {
        ERR("Failed to create components object");
        cJSON_Delete(root);
        return false;
    }

    // char unique_id[HA_BUFFER_SIZE];
    // char value_template[HA_BUFFER_SIZE];
    // char state_topic_str[HA_BUFFER_SIZE];

    int sensor_count = sensor_get_count();
    for (int i = 0; i < sensor_count; i++) {
        sensor_info_t info = sensor_get_info(i);
        cJSON *component = cJSON_CreateObject();
        if (!component) {
            ERR("Failed to create component object");
            continue;
        }

        cJSON_AddStringToObject(component, "platform", info.component_type);
        cJSON_AddStringToObject(component, "device_class",
info.device_class);

        if (snprintf(unique_id, sizeof(unique_id), "%s_%s", DEVICE_ID,
                info.unique_id) >= sizeof(unique_id)) {
            ERR("Unique ID truncated");
            cJSON_Delete(component);
            continue;
        }
        cJSON_AddStringToObject(component, "unique_id", unique_id);

        if (info.unit) {
            cJSON_AddStringToObject(
                component, "unit_of_measurement", info.unit);
        }
        if (info.icon) {
            cJSON_AddStringToObject(component, "icon", info.icon);
        }

        if (snprintf(value_template, sizeof(value_template),
                "{{ value_json.%s }}",
                info.unique_id) >= sizeof(value_template)) {
            ERR("Value template truncated");
            cJSON_Delete(component);
            continue;
        }
        cJSON_AddStringToObject(component, "value_template",
value_template);

        if (info.payload_on) {
            cJSON_AddStringToObject(component, "payload_on",
info.payload_on);
        }
        if (info.payload_off) {
            cJSON_AddStringToObject(component, "payload_off",
info.payload_off);
        }

        cJSON_AddItemToObject(components, info.name, component);
    }
    cJSON_AddItemToObject(root, "components", components);

    if (snprintf(state_topic_str, sizeof(state_topic_str),
"%s/device/%s/state", MQTT_TOPIC, DEVICE_ID) >= sizeof(state_topic_str)) {
        ERR("State topic truncated");
        cJSON_Delete(root);
        return false;
    }
    cJSON_AddStringToObject(root, "state_topic", state_topic_str);

    char *discovery_payload = cJSON_Print(root);
    bool success = false;

    if (discovery_payload) {
        printf("%s[%s] Publishing discovery: %s%s\n", col_p, sym_ha,
            discovery_payload, col_reset);

        success = broker_publish_wait(
            HA_TOPIC_DISCOVERY, discovery_payload, 1, 1, 2000);

        if (!success) {
            ERR("Discovery publish timeout");
        }
        free(discovery_payload);
    }

    cJSON_Delete(root);
    return success;
}

// #endif

*/