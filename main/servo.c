#include "servo.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include "esp_log.h"

static const char *TAG = "LAGP-SERVO";

const int freqHz = 50;

unsigned int min;
unsigned int max;

ledc_timer_bit_t timerResolution;

double ServoGetDutyByPercentage(double percentage)
{
	ESP_LOGI(TAG, "ServoGetDutyByPercentage");
	if (percentage <= 0){
		return 0;
	}

	if (percentage > 100){
		percentage = 100;
	}
	return (percentage / 100.0) * ((2<<(timerResolution-1))-1);
}

double ServoGetDutyByuS(double uS){
	ESP_LOGI(TAG, "ServoGetDutyByuS");
	return ServoGetDutyByPercentage(((uS * 100.0)/(1000000/freqHz)));
}

void ServoAttach(gpio_num_t pin, unsigned int minuS, unsigned int maxuS, ledc_channel_t ledcChannel, ledc_timer_t ledcTimer)
{
	ESP_LOGI(TAG, "ServoAttach");
	min = minuS;
	max = maxuS;
	timerResolution = LEDC_TIMER_16_BIT;

	ledc_timer_config_t timer_conf;
	timer_conf.duty_resolution 	= timerResolution;
	timer_conf.freq_hz    		= freqHz;
	timer_conf.speed_mode 		= LEDC_HIGH_SPEED_MODE;
	timer_conf.timer_num  		= ledcTimer;
	timer_conf.clk_cfg          = LEDC_AUTO_CLK;
	ledc_timer_config(&timer_conf);

	ledc_channel_config_t ledc_conf;
	ledc_conf.channel		= ledcChannel;
	ledc_conf.duty			= 0;
	ledc_conf.gpio_num		= (int)pin;
	ledc_conf.intr_type		= LEDC_INTR_DISABLE;
	ledc_conf.speed_mode	= LEDC_HIGH_SPEED_MODE;
	ledc_conf.timer_sel		= ledcTimer;
	ledc_conf.hpoint        = 0;
	ledc_channel_config(&ledc_conf);
}

void ServoDetach(ledc_channel_t ledcChannel)
{
	ESP_LOGI(TAG, "ServoDetach");
	ledc_stop(LEDC_HIGH_SPEED_MODE, ledcChannel, 0);
}

void ServoWriteMicroSeconds(ledc_channel_t ledcChannel, unsigned int uS)
{
	ESP_LOGI(TAG, "ServoWriteMicroSeconds");
	ledc_set_duty(LEDC_HIGH_SPEED_MODE, ledcChannel, ServoGetDutyByuS(uS));
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, ledcChannel);
}

void ServoWriteValue(ledc_channel_t ledcChannel, unsigned int value) 
{
	ESP_LOGI(TAG, "ServoWriteValue");
	// 0 = MinServoAngle ; 180 = Max ServoAngle;
	int scale = (value - 0) * (max - min) / (180 - 0) + min;
	ServoWriteMicroSeconds(ledcChannel, scale);
}