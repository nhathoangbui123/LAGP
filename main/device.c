#include "device.h"
#include "driver/gpio.h"
#include "esp_log.h"
static const char* TAG = "LAGP-DEVICE";
void DeviceInit(gpio_num_t gpio_num, gpio_mode_t mode)
{
    ESP_LOGI(TAG, "DeviceInit");
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(gpio_num, mode);   
}

bool DeviceGetLevel(gpio_num_t gpio_num)
{
    ESP_LOGI(TAG, "DeviceGetLevel");
    if(gpio_get_level(gpio_num)){
        return true;
    }else{
        return false;
    }
}

bool DeviceSetLevel(gpio_num_t gpio_num, uint32_t level)
{
    ESP_LOGI(TAG, "DeviceSetLevel");
    if(gpio_set_level(gpio_num, level)==ESP_OK){
        return true;
    }else{
        return false;
    }
}