#include <esp_log.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#include "rc522.h"
#include "dht11.h"
#include "mq135.h"
#include "servo.h"
#include "device.h"
#include "nextion.h"
#include "NVSDriver.h"

#include "wifi.h"

#include "common.h"

static const char* TAG = "LAGP-MAIN";

Uid uid;

spi_device_handle_t spi;
esp_mqtt_client_handle_t mqtt_client;
bool RC522Compare(uint8_t *buffer, uint8_t bufferSize) 
{

	for (uint8_t i = 0; i < bufferSize; i++) 
	{
		printf(" %x",buffer[i]);
        if(buffer[i]!=Param.id1[i]){
            Param.id1_flag=false;
            printf("id1_flag=false;\n");
            return false;
        }
	}
	printf("\n");
    return true;
}
void RC522Task(void* arg) 
{
	esp_err_t ret;
	spi_bus_config_t buscfg = {
			.miso_io_num = RC522_PIN_MISO,
			.mosi_io_num = RC522_PIN_MOSI,
			.sclk_io_num = RC522_PIN_CLK,
			.quadwp_io_num = -1,
			.quadhd_io_num = -1
	};
	spi_device_interface_config_t devcfg = {
			.clock_speed_hz = 1*1000*1000, //Clock out at 10 MHz
			.mode = 0, //SPI mode 0
			.spics_io_num = RC522_PIN_CS, //CS pin
			.queue_size = 8, //We want to be able to queue 7 transactions at a time
			//.pre_cb=ili_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
	};
	//Initialize the SPI bus
	ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
	assert(ret == ESP_OK);
	//Attach the RFID to the SPI bus
	ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
	assert(ret == ESP_OK);
	PCD_Init();
	uint8_t r ;
    Param.id1[0]=0x81;
    Param.id1[1]=0x52;
    Param.id1[2]=0x62;
    Param.id1[3]=0x1C;
	while (1) {
		r = PICC_IsNewCardPresent();
		if (r) 
		{
			if (PICC_ReadCardSerial()) 
			{
				if(RC522Compare(&uid.uidByte, uid.size)){
                    printf("card match\n");
                }else{
                    printf("card not match\n");
                }
				//PICC_DumpToSerial(&uid);
			}
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
void RainTask(void* arg){
	DeviceInit(RAIN_PIN, GPIO_MODE_INPUT);
	while(1){
		SensorData.rain = DeviceGetLevel(RAIN_PIN);
		ESP_LOGI(TAG, "SensorData.rain = %d", SensorData.rain);
		vTaskDelay(3000 / portTICK_PERIOD_MS);
	}
}
void ReadNVS(void){
	NVSDriverOpen("nvs");

    char* Sdev1 = NULL;
    NVSDriverRead("Sdev1", &Sdev1);
    if(Sdev1!=NULL){
        device.Fan1State = atoi(Sdev1);
    }
    free(Sdev1);

    char* Sdev2 = NULL;
    NVSDriverRead("Sdev2", &Sdev2);
    if(Sdev2!=NULL){
        device.Fan2State = atoi(Sdev2);
    }
    free(Sdev2);

    char* Sdev3 = NULL;
    NVSDriverRead("Sdev3", &Sdev3);
    if(Sdev3!=NULL){
        device.LED1State = atoi(Sdev3);
    }
    free(Sdev3);

    char* Sdev4 = NULL;
    NVSDriverRead("Sdev4", &Sdev4);
    if(Sdev4!=NULL){
        device.LED2State = atoi(Sdev4);
    }
    free(Sdev4);

	if(NVSDriverRead("WN", &Param.WN)!=true){
        Param.WN = "1";
    }
    if(NVSDriverRead("WP", &Param.WP)!=true){
        Param.WP = "1";
    }

    NVSDriverClose();
}
void DeviceTask(void *arg){
	DeviceInit(FAN1, GPIO_MODE_OUTPUT);
	DeviceInit(FAN2, GPIO_MODE_OUTPUT);
	DeviceInit(LED1, GPIO_MODE_OUTPUT);
	DeviceInit(LED2, GPIO_MODE_OUTPUT);
	while (1)
	{
		if(device.LED1State!=device.LED1StateO){
			if(device.LED1State){
				DeviceSetLevel(LED1, ON);
			}else{
				DeviceSetLevel(LED1, OFF);
			}
			device.LED1StateO = device.LED1State;
		}

		if(device.LED2State!=device.LED2StateO){
			if(device.LED2State){
				DeviceSetLevel(LED2, ON);
			}else{
				DeviceSetLevel(LED2, OFF);
			}
			device.LED2StateO = device.LED2State;
		}

		if(device.Fan1State!=device.Fan1StateO){
			if(device.Fan1State){
				DeviceSetLevel(FAN1, ON);
			}else{
				DeviceSetLevel(FAN1, OFF);
			}
			device.Fan1StateO = device.Fan1State;
		}

		if(device.Fan2State!=device.Fan2StateO){
			if(device.Fan2State){
				DeviceSetLevel(FAN2, ON);
			}else{
				DeviceSetLevel(FAN2, OFF);
			}
			device.Fan2StateO = device.Fan2State;
		}

		vTaskDelay(10/ portTICK_PERIOD_MS);
	}
}
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    mqtt_client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, IO_TOPIC, "1000", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, LED1_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, LED2_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

			msg_id = esp_mqtt_client_subscribe(client, FAN1_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, FAN2_TOPIC, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

			char topic[20];
			char data[4];
			sprintf(topic, "%.*s\r\n", event->topic_len, event->topic);
			// ESP_LOGI(TAG, "topic = %s", topic);
			// ESP_LOGI(TAG, "len = %d", strlen(topic));

			if(strcmp(topic, "nhathoang/feeds/led1\r\n") == 0){
				ESP_LOGI(TAG, "nhathoang/feeds/led1");
				sprintf(data, "%.*s\r\n", event->data_len, event->data);
				printf("data=%s", data);
				if(strcmp(data, "0\r\n") == 0){
					device.LED1State = 0;
				}else{
					device.LED1State = 1;
				}
			}else if(strcmp(topic,"nhathoang/feeds/led2\r\n") == 0){
				ESP_LOGI(TAG, "nhathoang/feeds/led2");
				sprintf(data, "%.*s\r\n", event->data_len, event->data);
				printf("data=%s", data);
				if(strcmp(data, "0\r\n") == 0){
					device.LED2State = 0;
				}else{
					device.LED2State = 1;
				}
			}else if(strcmp(topic,"nhathoang/feeds/fan1\r\n") == 0){
				ESP_LOGI(TAG, "nhathoang/feeds/fan1");
				sprintf(data, "%.*s\r\n", event->data_len, event->data);
				printf("data=%s", data);
				if(strcmp(data, "0\r\n") == 0){
					device.Fan1State = 0;
				}else{
					device.Fan1State = 1;
				}
			}else if(strcmp(topic,"nhathoang/feeds/fan2\r\n") == 0){
				ESP_LOGI(TAG, "nhathoang/feeds/fan2");
				sprintf(data, "%.*s\r\n", event->data_len, event->data);
				printf("data=%s", data);
				if(strcmp(data, "0\r\n") == 0){
					device.Fan2State = 0;
				}else{
					device.Fan2State = 1;
				}
			}
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}
static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = BROKER_URL,
        .event_handle = mqtt_event_handler,
        // .user_context = (void *)your_context
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}
void app_main()
{
	ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

	/* Initialize NVS partition */
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_init());
	}
	//Read NVS 
	ReadNVS();
	//wifi pre setup
	Param.WN = "CODER HOUSE";
	Param.WP = "88888888";

	ServoAttach(SERVO1, 400, 2600, LEDC_CHANNEL_0, LEDC_TIMER_0);
	ServoAttach(SERVO2, 400, 2600, LEDC_CHANNEL_1, LEDC_TIMER_1);

	//Init Nextion HMI
    NextionInit();
    //Nextion RX Task
    xTaskCreate(&NextionRXTask, "NextionRXTask", 1024*2, NULL, 1, NULL);
    //Nextion TX Task
    xTaskCreate(&NextionTXTask, "NextionTXTask", 1024*2, NULL, 2, NULL);
	//Device control Task
	xTaskCreate(&DeviceTask, "DeviceTask", 1024*2, NULL, 4, NULL);
	if(wifi_init_sta()){
		mqtt_app_start();
		// DHT11Run();
		// MQ135Run();
		// xTaskCreate(&RainTask, "RainTask", 8192, NULL, 5, NULL);
		// xTaskCreate(&RC522Task, "RC522Task", 8192, NULL, 5, NULL);
	}
}