#ifndef __COMMON__
#define __COMMON__

#include "stdint.h"
#include "stdbool.h" 
#include "esp_log.h"
#include "driver/gpio.h"

#define DHT11_PIN               GPIO_NUM_5

#define RC522_PIN_MISO          GPIO_NUM_25
#define RC522_PIN_MOSI          GPIO_NUM_23
#define RC522_PIN_CLK           GPIO_NUM_19
#define RC522_PIN_CS            GPIO_NUM_22
#define RC522_PIN_RST           GPIO_NUM_14

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
}SensorData_t;

SensorData_t SensorData;

#ifdef __cplusplus
}
#endif

#endif