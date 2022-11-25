#ifndef __SERVO__
#define __SERVO__

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>

#ifdef __cplusplus
extern "C" {
#endif

double ServoGetDutyByPercentage(double percentage);
double ServoGetDutyByuS(double uS);

void ServoAttach(gpio_num_t pin, unsigned int minuS, unsigned int maxuS, ledc_channel_t ledcChannel, ledc_timer_t ledcTimer);
void ServoWriteMicroSeconds(ledc_channel_t ledcChannel, unsigned int uS);
void ServoWriteValue(ledc_channel_t ledcChannel, unsigned int value);
void ServoDetach(ledc_channel_t ledcChannel);

#ifdef __cplusplus
}
#endif

#endif