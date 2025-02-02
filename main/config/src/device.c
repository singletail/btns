#include "device.h"
#include "sensor.h"

// add sensors here following homeassistant schema
// this is read by init.c and sent to sensor.c

// TODO: rewrite this.

void device_init(void) {

    int presence_id = sensor_add("presence", // name
        "presence",                          // unique_id
        "binary_sensor",                     // component_type
        "occupancy",                         // device_class
        NULL,                                // unit of measurement
        "mdi:motion-sensor",                 // icon
        "true",                              // payload_on
        "false",                             // payload_off
        VAL_TYPE_BOOL);

    // probably move this:
    sensor_update_bool(presence_id, false);
    bits_on(BIT_SENSOR_OK);
}
