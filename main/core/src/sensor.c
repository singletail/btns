#include "sensor.h"

#define MAX_SENSORS 16

static sensor_t sensors[MAX_SENSORS] = {0};
static uint8_t sensor_count = 0;

int sensor_add(
    const char *name, const char *unique_id, const char *component_type, const char *device_class,
    const char *unit, const char *icon, const char *payload_on, const char *payload_off,
    value_type_t type) {
    LOG("sensor_add()", unique_id);
        if (sensor_count >= MAX_SENSORS) {
            ERR("Cannot add sensor - max count reached.");
            INFO("(Increase MAX_SENSORS in sensor.c)");
            return -1;
    }

        if (!name || !unique_id || !component_type) {
            ERR("Required sensor parameters missing");
            return -1;
    }

    sensor_t *s = &sensors[sensor_count];
    memset(s, 0, sizeof(sensor_t));

    // Required fields - check allocation success
    s->name = strdup(name);
    if (!s->name) goto cleanup;

    s->unique_id = strdup(unique_id);
    if (!s->unique_id) goto cleanup;

    s->component_type = strdup(component_type);
    if (!s->component_type) goto cleanup;

        if (device_class) {
            s->device_class = strdup(device_class);
            if (!s->device_class) goto cleanup;
    }

        if (unit) {
            s->unit = strdup(unit);
            if (!s->unit) goto cleanup;
    }

        if (icon) {
            s->icon = strdup(icon);
            if (!s->icon) goto cleanup;
    }

        if (payload_on) {
            s->payload_on = strdup(payload_on);
            if (!s->payload_on) goto cleanup;
    }

        if (payload_off) {
            s->payload_off = strdup(payload_off);
            if (!s->payload_off) goto cleanup;
    }

    s->value.type = type;
    s->enabled = true;
    int id = sensor_count;
    sensor_count++;
    return id;

cleanup:
    ERR("Memory allocation failed in sensor_add");
    if (s->name) free(s->name);
    if (s->unique_id) free(s->unique_id);
    if (s->component_type) free(s->component_type);
    if (s->device_class) free(s->device_class);
    if (s->unit) free(s->unit);
    if (s->icon) free(s->icon);
    if (s->payload_on) free(s->payload_on);
    if (s->payload_off) free(s->payload_off);
    memset(s, 0, sizeof(sensor_t));
    return -1;
}

void sensor_update_bool(int sensor_id, bool value) {
        if (sensor_id >= 0 && sensor_id < sensor_count) {
            sensor_t *s = &sensors[sensor_id];
                if (s->value.type == VAL_TYPE_BOOL) {
                    s->value.value.b = value;
                    s->last_update = esp_timer_get_time();
            }
    }
    DEBUG("sensor_update_bool() sensor %d, value %d", sensor_id, value);
}

void sensor_update_int(int sensor_id, int value) {
        if (sensor_id >= 0 && sensor_id < sensor_count) {
            sensor_t *s = &sensors[sensor_id];
                if (s->value.type == VAL_TYPE_INT) {
                    s->value.value.i = value;
                    s->last_update = esp_timer_get_time();
            }
    }
    DEBUG("sensor_update_int() sensor %d, value %d", sensor_id, value);
}

void sensor_update_float(int sensor_id, float value) {
        if (sensor_id >= 0 && sensor_id < sensor_count) {
            sensor_t *s = &sensors[sensor_id];
                if (s->value.type == VAL_TYPE_FLOAT) {
                    s->value.value.f = value;
                    s->last_update = esp_timer_get_time();
            }
    }
    DEBUG("sensor_update_float() sensor %d, value %f", sensor_id, value);
}

void sensor_update_string(int sensor_id, const char *value) {
        if (sensor_id >= 0 && sensor_id < sensor_count) {
            sensor_t *s = &sensors[sensor_id];
                if (s->value.type == VAL_TYPE_STRING) {
                        if (s->value.value.s) {
                            free(s->value.value.s);
                    }
                    s->value.value.s = strdup(value);
                    s->last_update = esp_timer_get_time();
            }
    }
    DEBUG("sensor_update_string() sensor %d, value %s", sensor_id, value);
}

sensor_info_t sensor_get_info(int sensor_id) {
    sensor_info_t info = {0};
        if (sensor_id >= 0 && sensor_id < sensor_count) {
            sensor_t *s = &sensors[sensor_id];
            info.name = s->name;
            info.unique_id = s->unique_id;
            info.component_type = s->component_type;
            info.device_class = s->device_class;
            info.unit = s->unit;
            info.icon = s->icon;
            info.payload_on = s->payload_on;
            info.payload_off = s->payload_off;
    }
    return info;
}

value_type_t sensor_get_type(int sensor_id) {
        if (sensor_id >= 0 && sensor_id < sensor_count) {
            return sensors[sensor_id].value.type;
    }
    return VAL_TYPE_BOOL;    // Default
}

value_t sensor_get_value(int sensor_id) {
    value_t empty = {0};
        if (sensor_id >= 0 && sensor_id < sensor_count) {
            return sensors[sensor_id].value.value;
    }
    return empty;
}

int sensor_get_count(void) { return sensor_count; }
