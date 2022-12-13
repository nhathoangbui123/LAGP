#ifndef __nextion__
#define __nextion__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "soc/uart_struct.h"
#include "string.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static const int RX_BUFFER = 1024;
static const int TX_BUFFER = 256;

int NextionSendData(const char* logName, const char* data);
void NextionInit();
void NextionTXTask(void* arg);
void NextionRXTask(void* arg);

#ifdef __cplusplus
}
#endif
#endif //__nextion__
