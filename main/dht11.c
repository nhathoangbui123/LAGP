#include "dht11.h"
#include "common.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static gpio_num_t dht_gpio;
static int64_t last_read_time = -2000000;
static struct DHT11Data_t last_read;

static const char *TAG = "LAGP-DHT11";

static esp_err_t DHT11WaitOrTimeout(uint16_t microSeconds, int level) 
{
    //ESP_LOGI(TAG, "DHT11WaitOrTimeout");
    int micros_ticks = 0;
    while(gpio_get_level(dht_gpio) == level) { 
        if(micros_ticks++ > microSeconds) 
            return ESP_ERR_TIMEOUT;
        ets_delay_us(1);
    }
    return micros_ticks;
}

static esp_err_t DHT11CheckCRC(uint8_t data[]) 
{
    //ESP_LOGI(TAG, "DHT11CheckCRC");
    if(data[4] == (data[0] + data[1] + data[2] + data[3]))
        return ESP_OK;
    else
        return ESP_ERR_INVALID_CRC;
}

static void DHT11SendStartSignal() 
{
    //ESP_LOGI(TAG, "DHT11SendStartSignal");
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20 * 1000);
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(40);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

static esp_err_t DHT11CheckRespone() 
{
    //ESP_LOGI(TAG, "DHT11CheckRespone");
    /* Wait for next step ~80us*/
    if(DHT11WaitOrTimeout(80, 0) == ESP_ERR_TIMEOUT )
        return ESP_ERR_TIMEOUT ;

    /* Wait for next step ~80us*/
    if(DHT11WaitOrTimeout(80, 1) == ESP_ERR_TIMEOUT ) 
        return ESP_ERR_TIMEOUT ;

    return ESP_OK;
}

void DHT11Init(gpio_num_t gpio_num) 
{
    ESP_LOGI(TAG, "DHT11Init");
    /* Wait 1 seconds to make the device pass its initial unstable status */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    dht_gpio = gpio_num;
}

esp_err_t DHT11Read(struct DHT11Data_t* readData) 
{
    ESP_LOGI(TAG, "DHT11Read");
    /* Tried to sense too son since last read (dht11 needs ~2 seconds to make a new read) */
    if(esp_timer_get_time() - 2000000 < last_read_time) {
        readData->temperature = last_read.temperature;
        readData->humidity = last_read.humidity;
        return ESP_OK;
    }

    last_read_time = esp_timer_get_time();

    uint8_t data[5] = {0,0,0,0,0};

    DHT11SendStartSignal();

    if(DHT11CheckRespone() == ESP_ERR_TIMEOUT)
        return ESP_ERR_TIMEOUT;
    
    /* Read response */
    for(int i = 0; i < 40; i++) {
        /* Initial data */
        if(DHT11WaitOrTimeout(50, 0) == ESP_ERR_TIMEOUT)
            return ESP_ERR_TIMEOUT;
                
        if(DHT11WaitOrTimeout(70, 1) > 28) {
            /* Bit received was a 1 */
            data[i/8] |= (1 << (7-(i%8)));
        }
    }
    
    if(DHT11CheckCRC(data) != ESP_ERR_INVALID_CRC) {
        last_read.temperature = data[2];
        last_read.humidity = data[0];
        readData->temperature = last_read.temperature;
        readData->humidity = last_read.humidity;
        return ESP_OK;
    } else {
        return ESP_ERR_INVALID_CRC;
    }
}

void DHT11Task(void *arg)
{
    ESP_LOGI(TAG, "DHT11Task");
    DHT11Init(DHT11_PIN);
    struct DHT11Data_t dht11_data;
    while(1)
    {
        DHT11Read(&dht11_data);
        ESP_LOGI(TAG, "Temperature: %d", dht11_data.temperature);
        SensorData.temperature = dht11_data.temperature;
        ESP_LOGI(TAG, "Humidity: %d", dht11_data.humidity);
        SensorData.humidity = dht11_data.humidity;
        vTaskDelay(3000 / portTICK_RATE_MS);
    }
}

void DHT11Run(void)
{
    xTaskCreate(&DHT11Task, "DHT11Task", 2048, NULL, 5, NULL);
}