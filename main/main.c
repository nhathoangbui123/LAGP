#include <esp_log.h>
#include <inttypes.h>
#include "nvs_flash.h"

#include "rc522.h"
#include "dht11.h"
#include "servo.h"
#include "wifi.h"
#include "common.h"

static const char* TAG = "LAGP-MAIN";

Uid uid;

spi_device_handle_t spi;

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

void app_main()
{
	/* Initialize NVS partition */
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_init());
	}
	//wifi pre setup
	Param.WN="NhatHoang";
	Param.WP="01217818548";
	wifi_init_sta();

	// attach(GPIO_NUM_18,400,2600,LEDC_CHANNEL_0,LEDC_TIMER_0);
    // write_val(0);	
    // vTaskDelay(1000/ portTICK_RATE_MS);
	// write_val(90);
    xTaskCreate(&RC522Task, "RC522Task", 8192, NULL, 5, NULL);
}