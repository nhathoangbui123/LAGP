#include "cmd.h"
#include "NVSDriver.h"
#include "nextion.h"
#include "device.h"
enum 
{
    UNKNOWN_CMD,
    FAN1ON,               
    FAN1OFF,  
    FAN2ON,               
    FAN2OFF,
    LED1ON,               
    LED1OFF,
    LED2ON,               
    LED2OFF,
    ALL_ON,
    ALL_OFF,
    WN,
    WP,
    RST,
};

char *CMD_STRINGS[] = {
    "  ",                   //UNKNOWN_CMD
    "FAN1ON",               
    "FAN1OFF",  
    "FAN2ON",               
    "FAN2OFF",
    "LED1ON",               
    "LED1OFF",
    "LED2ON",               
    "LED2OFF",
    "ALL_ON",
    "ALL_OFF",
    "WN",
    "WP",
    "RST",
};

static const char *CMD_TAG = "LAGP-Command";

#define __NUMBER_OF_CMD_STRINGS (sizeof(CMD_STRINGS) / sizeof(*CMD_STRINGS))
int GetCmd(char *text)
{
	int i = 0;
	for (i = 0; i < __NUMBER_OF_CMD_STRINGS; i++) {
		if (strcmp(text, CMD_STRINGS[i]) == 0) {
			return i;
		}
	}
	return UNKNOWN_CMD;
}
int ParseCmd(char *text, char* body)
{
  ESP_LOGD(CMD_TAG,"body: %s", body);
	int cmd_index = GetCmd(text);
    switch (cmd_index) {
      case FAN1ON:
        ESP_LOGD(CMD_TAG, "FAN1ON");
        DeviceSetLevel(FAN1, ON);

        device.Fan1State = 1;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev1", "1");
        NVSDriverClose();
        return 0;
      break;
      case FAN1OFF:
        ESP_LOGD(CMD_TAG, "FAN1OFF");
        DeviceSetLevel(FAN1, OFF);

        device.Fan1State = 0;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev1", "0");
        NVSDriverClose();
        return 0;
      break;
      case FAN2ON:
        ESP_LOGD(CMD_TAG, "FAN2ON");
        DeviceSetLevel(FAN2, ON);

        device.Fan2State = 1;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev2", "1");
        NVSDriverClose();
        return 0;
      break;
      case FAN2OFF:
        ESP_LOGD(CMD_TAG, "FAN2OFF");
        DeviceSetLevel(FAN2, OFF);

        device.Fan2State = 0;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev2", "0");
        NVSDriverClose();
        return 0;
      break;
      case LED1ON:
        ESP_LOGD(CMD_TAG, "LED1ON");
        DeviceSetLevel(LED1, ON);

        device.LED1State = 1;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev3", "1");
        NVSDriverClose();
        return 0;
      break;
      case LED1OFF:
        ESP_LOGD(CMD_TAG, "LED1OFF");
        DeviceSetLevel(LED1, OFF);

        device.LED1State = 0;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev3", "0");
        NVSDriverClose();
        return 0;
      break;
      case LED2ON:
        ESP_LOGD(CMD_TAG, "LED2ON");
        DeviceSetLevel(LED2, ON);

        device.LED2State = 1;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev4", "1");
        NVSDriverClose();
        return 0;
      break;
      case LED2OFF:
        ESP_LOGD(CMD_TAG, "LED2OFF");
        DeviceSetLevel(LED2, OFF);

        device.LED2State = 0;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev4", "0");
        NVSDriverClose();
        return 0;
      break;
      case ALL_ON:
        ESP_LOGD(CMD_TAG,"ALL_ON");
        DeviceSetLevel(FAN1, ON);
        DeviceSetLevel(FAN2, ON);
        DeviceSetLevel(LED1, ON);
        DeviceSetLevel(LED2, ON);

        device.Fan1State = 1;
        device.Fan2State = 1;
        device.LED1State = 1;
        device.LED2State = 1;

        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev1", "1");
        NVSDriverWrite("Sdev2", "1");
        NVSDriverWrite("Sdev3", "1");
        NVSDriverWrite("Sdev4", "1");
        NVSDriverClose();
        return 0;
      break;
      case ALL_OFF:
        ESP_LOGD(CMD_TAG,"ALL_OFF");
        DeviceSetLevel(FAN1, OFF);
        DeviceSetLevel(FAN2, OFF);
        DeviceSetLevel(LED1, OFF);
        DeviceSetLevel(LED2, OFF);

        device.Fan1State = 0;
        device.Fan2State = 0;
        device.LED1State = 0;
        device.LED2State = 0;
        
        NVSDriverOpen("nvs");
        NVSDriverWrite("Sdev1", "0");
        NVSDriverWrite("Sdev2", "0");
        NVSDriverWrite("Sdev3", "0");
        NVSDriverWrite("Sdev4", "0");
        NVSDriverClose();
        return 0;
      break;
      case WN:
        ESP_LOGD(CMD_TAG,"WN");

        NVSDriverOpen("nvs");
        NVSDriverWrite("WN", body);
        NVSDriverClose();

        Param.WN=strdup(body);
      break;
      case WP:
        ESP_LOGD(CMD_TAG,"WP");
        
        NVSDriverOpen("nvs");
        NVSDriverWrite("WP", body);
        NVSDriverClose();

        Param.WP=strdup(body);
      break;
      case RST:
        ESP_LOGD(CMD_TAG,"RST");
        ESP_LOGD(CMD_TAG,"ESP32 Will restart");
        char *cmd = (char*)malloc(256);
        for(int i=0;i<=100;i++){
          sprintf(cmd, "ProgressReset.j0.val=%d\xFF\xFF\xFF",i);
          NextionSendData(CMD_TAG, cmd);
          vTaskDelay(10/ portTICK_PERIOD_MS);
        }
        esp_restart();
      break;
      case UNKNOWN_CMD:
        ESP_LOGD(CMD_TAG,"UART RX: cmd UNKNOWN_CMD");
      return 1;
      break;
	}
	return 0;
}