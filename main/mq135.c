#include "mq135.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

static const char *TAG = "LAGP-MQ135";
static esp_adc_cal_characteristics_t adc1_chars;
static int adc_raw;
static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ADC_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}

void MQ135Task(void *arg){
    uint32_t voltage = 0;
    bool cali_enable = adc_calibration_init();

    //ADC1 config
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_MQ135, ADC_ATTEN));

    while (1) {
        adc_raw = adc1_get_raw(ADC1_MQ135);
        ESP_LOGI(TAG, "raw  data: %d", adc_raw);
        if (cali_enable) {
            voltage = esp_adc_cal_raw_to_voltage(adc_raw, &adc1_chars);
            ESP_LOGI(TAG, "cali data: %d mV", voltage);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void MQ135Run(void){
    xTaskCreate(&MQ135Task, "MQ135Task", 2048, NULL, 5, NULL);
}