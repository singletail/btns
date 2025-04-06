#ifndef _DEFINES_H
#define _DEFINES_H

#define BITS_READY       BIT0
#define BIT_PINS_READY   BIT1
#define BIT_NET_INIT     BIT2
#define BIT_NET_FAIL     BIT3
#define BIT_NET_READY    BIT4
#define BIT_MQTT_READY   BIT5
#define BIT_NTP_READY    BIT6
#define BIT_HA_READY     BIT7
#define BIT_NET_OK       BIT8

// #define PIN_ONBOARD_LED 48 // longboi

#define CONFIG_RMT_ENABLE_DEBUG_LOG 1

#define PIN_SEG7_CLK 4
#define PIN_SEG7_DIO 5

// Wearable
// #define LED_PIN_1 43
// #define LED_PIN_2 6
// #define LED_PIN_3 5

#define STRIP_NUM_STRIPS 1

// Longboi
#define STRIP_PIN_1 42
#define STRIP_PIN_2 41
#define STRIP_PIN_3 40
#define STRIP_PIN_4 39

#define STRIP_LENGTH_1 256
#define STRIP_LENGTH_2 46
#define STRIP_LENGTH_3 46
#define STRIP_LENGTH_4 46

#define ENABLE_WIFI

#endif