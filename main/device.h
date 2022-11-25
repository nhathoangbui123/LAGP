#ifndef __DEVICE__
#define __DEVICE__

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

void DeviceInit(gpio_num_t gpio_num, gpio_mode_t mode);
bool DeviceGetLevel(gpio_num_t gpio_num);
bool DeviceSetLevel(gpio_num_t gpio_num, uint32_t level);

#ifdef __cplusplus
}
#endif

#endif