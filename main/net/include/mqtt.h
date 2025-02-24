#ifndef _MQTT_H
#define _MQTT_H

#include "includes.h"
#include "mqtt_client.h"

void mqtt_status(esp_err_t status);
void mqtt_init(void);

void mqtt_connected(void);
int mqtt_publish(const char *topic, const char *payload, int qos, int retain);
void mqtt_event_data(esp_mqtt_event_t *event);
void mqtt_error(esp_mqtt_event_t *event);

#endif