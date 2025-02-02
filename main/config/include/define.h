#ifndef _DEFINES_H
#define _DEFINES_H

#include "config.h"
#include "include.h"

#define BIT_GPIO_OK BIT0            // Pin initialized
#define BIT_NET_INIT BIT1           // Network stack initialized
#define BIT_NET_OK BIT2             // WiFi connected with IP
#define BIT_NET_FAIL BIT3           // WiFi disconnected
#define BIT_TIME_INIT BIT4          // Time initialized
#define BIT_TIME_BUSY BIT5          // Connecting.
#define BIT_TIME_OK BIT6            // Clock initialized
#define BIT_MQTT_OK BIT7            // MQTT connected
#define BIT_SENSOR_OK BIT8          // Sensor initialized
#define BIT_HA_OK BIT11             // Home Assistant initialized
#define BIT_NET_STACK_OK BIT12      // Network stack initialized
#define BIT_PRESENCE_DETECTED BIT13 // mmwave
#define BIT_TEST_21 BIT21           // Test bit
#define BIT_TEST_22 BIT22           // Test bit
#define BIT_TEST_23 BIT23           // Test bit

#define DEFAULT_TASK_SIZE 2048
#define U_TO_MS 1000

// #define PIN_LED_1 42
// #define LED_LENGTH_1 300
// #define PIN_SEG7_CLK 4
// #define PIN_SEG7_DIO 5
// #define PIN_MMWAVE 2       // longboi
// #define PIN_MMWAVE_LED 1  // longboi


#define PIN_MMWAVE 18       // c6
#define PIN_MMWAVE_LED 17   // c6


/*
#ifdef _XIAO
#define ENABLE_MMWAVE
#define PIN_MMWAVE 43
#define PIN_MMWAVE_LED 21 // onboard
#endif
*/
#endif