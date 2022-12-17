#ifndef __COMMON__
#define __COMMON__

#include "stdint.h"
#include "stdbool.h" 
#include "esp_log.h"
#include "driver/gpio.h"

#define BROKER_URL "mqtt://nhathoang:a3898eade09f42659ddfb1dd2c438b0c@io.adafruit.com"
#define IO_TOPIC "nhathoang/feeds/test"

#define LED1_TOPIC "nhathoang/feeds/led1"
#define LED2_TOPIC "nhathoang/feeds/led2"
#define FAN1_TOPIC "nhathoang/feeds/fan1"
#define FAN2_TOPIC "nhathoang/feeds/fan2"

// #define MQ135_PIN               GPIO_NUM_34
#define DHT11_PIN               GPIO_NUM_5
#define RAIN_PIN                GPIO_NUM_18

#define SERVO1                  GPIO_NUM_2
#define SERVO2                  GPIO_NUM_4

#define FAN1                    GPIO_NUM_26
#define FAN2                    GPIO_NUM_27
#define LED1                    GPIO_NUM_32
#define LED2                    GPIO_NUM_33

#define RC522_PIN_MISO          GPIO_NUM_25
#define RC522_PIN_MOSI          GPIO_NUM_23
#define RC522_PIN_CLK           GPIO_NUM_19
#define RC522_PIN_CS            GPIO_NUM_22
#define RC522_PIN_RST           GPIO_NUM_14

#define TX_HMI                  GPIO_NUM_17
#define RX_HMI                  GPIO_NUM_16

#define UART_HMI	            UART_NUM_2

#define OFF                     1
#define ON                      0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t id1[4];
    bool id1_flag;
    // wifi credentials
    char* WN;
    char* WP;
}Param_t;
Param_t Param;

typedef struct
{
    int temperature;
    int humidity;
    int mq135;
    int rain;
}SensorData_t;

SensorData_t SensorData;

typedef struct
{
    int Fan1State;
    int Fan1StateO;
    int Fan2State;
    int Fan2StateO;
    int LED1State;
    int LED1StateO;
    int LED2State;
    int LED2StateO;
}DevState_t;

DevState_t device;

#ifdef __cplusplus
}
#endif

#endif