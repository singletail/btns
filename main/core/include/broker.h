#ifndef _broker_H
#define _broker_H

#include "include.h"
#include "mqtt_client.h"

void broker_status(esp_err_t status);
void broker_init(void);

void broker_connected(void);
int broker_publish(
    const char *topic, const char *payload, int qos, int retain);
void broker_event_data(esp_mqtt_event_t *event);
void broker_error(esp_mqtt_event_t *event);

#endif