#ifndef SENSOR_H
#define SENSOR_H

#include "includes.h"

typedef enum
{
    VAL_TYPE_BOOL,
    VAL_TYPE_INT,
    VAL_TYPE_FLOAT,
    VAL_TYPE_STRING
} value_type_t;

// Structure to hold sensor metadata
typedef struct {
    const char *name;
    const char *unique_id;
    const char *component_type;
    const char *device_class;
    const char *unit;
    const char *icon;
    const char *payload_on;
    const char *payload_off;
} sensor_info_t;

typedef union {
    bool b;
    int i;
    float f;
    char *s;
} value_t;

typedef struct {
    value_type_t type;
    value_t value;
} device_value_t;

typedef struct {
    char *name;
    char *unique_id;
    char *component_type;    // binary_sensor, sensor, etc.
    char *device_class;      // temperature, humidity, etc.
    char *unit;              // °C, %, etc.
    char *icon;              // mdi:thermometer, etc.
    char *payload_on;
    char *payload_off;
    bool enabled;
    device_value_t value;
    uint64_t last_update;
} sensor_t;

int sensor_add(
    const char *name, const char *unique_id, const char *component_type, const char *device_class,
    const char *unit, const char *icon, const char *payload_on, const char *payload_off,
    value_type_t value_type);
void sensor_update_bool(int sensor_id, bool value);
void sensor_update_int(int sensor_id, int value);
void sensor_update_float(int sensor_id, float value);
void sensor_update_string(int sensor_id, const char *value);

sensor_info_t sensor_get_info(int sensor_id);
value_type_t sensor_get_type(int sensor_id);
value_t sensor_get_value(int sensor_id);
int sensor_get_count(void);

#endif
