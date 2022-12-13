#ifndef __CMD__
#define __CMD__

#include "common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

int ParseCmd(char *text, char* body);
int GetCmd(char *text);

#ifdef __cplusplus
}
#endif
#endif