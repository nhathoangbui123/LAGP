#ifndef _MQ135_H_
#define _MQ135_H_
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

//ADC Channels
#define ADC1_MQ135          ADC1_CHANNEL_6  //GPIO_NUM_34
//ADC Attenuation
#define ADC_ATTEN           ADC_ATTEN_DB_11
//ADC Calibration
#define ADC_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_VREF

void MQ135Run(void);

#endif /* _MQ135_H_ */