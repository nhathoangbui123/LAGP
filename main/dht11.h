#ifndef __DHT11__
#define __DHT11__

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

enum dht11_status {
    DHT11_CRC_ERROR = -2, 
    DHT11_TIMEOUT_ERROR, 
    DHT11_OK
};

struct DHT11Data_t {
    int temperature;
    int humidity;
};

void DHT11Init(gpio_num_t);

esp_err_t DHT11Read(struct DHT11Data_t* readData);

void DHT11Run(void);

#ifdef __cplusplus
}
#endif

#endif /* __DHT11__*/