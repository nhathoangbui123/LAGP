#include "nextion.h"
#include "common.h"
#include "device.h"
#include "cJSON.h"
#include "cmd.h"

static const char *TX_TASK_TAG = "LAGP-NextionTXTask";
static const char *RX_TASK_TAG = "LAGP-NextionRXTask";

void NextionInit() {
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_HMI, &uart_config);
    uart_set_pin(UART_HMI, TX_HMI, RX_HMI, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_HMI, RX_BUFFER * 2, 0, 0, NULL, 0);
}

int NextionSendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_HMI, data, len);
    ESP_LOGD(logName, "Wrote %d bytes", txBytes);
    ESP_LOGD(logName, "Sent %s", data);
    return txBytes;
}

void NextionTXTask(void* arg)
{
    ESP_LOGD(TX_TASK_TAG, "NEXTION TX TASK");
    char *cmd = (char*)malloc(TX_BUFFER);
    while (1) {
        ESP_LOGD(TX_TASK_TAG, "Free memory: %d bytes", esp_get_free_heap_size());

        sprintf(cmd, "Monitor.power_v.txt=\"%d*C\"\xFF\xFF\xFF", SensorData.temperature);
        ESP_LOGD(TX_TASK_TAG, "Monitor.power_v.txt=%d*C", SensorData.temperature);
        NextionSendData(TX_TASK_TAG, cmd);	

        sprintf(cmd, "Monitor.freq_v.txt=\"%d%%RH\"\xFF\xFF\xFF", SensorData.humidity);
        ESP_LOGD(TX_TASK_TAG, "Monitor.freq_v.txt=%d%%RH", SensorData.humidity);
        NextionSendData(TX_TASK_TAG, cmd);

        sprintf(cmd, "Monitor.vol_v.txt=\"%d\"\xFF\xFF\xFF", SensorData.mq135);
        ESP_LOGD(TX_TASK_TAG, "Monitor.vol_v.txt=%d", SensorData.mq135);
        NextionSendData(TX_TASK_TAG, cmd);

        sprintf(cmd, "Monitor.amp_v.txt=\"%d\"\xFF\xFF\xFF", SensorData.rain);
        ESP_LOGD(TX_TASK_TAG, "Monitor.amp_v.txt=%d", SensorData.rain);
        NextionSendData(TX_TASK_TAG, cmd);

        //wifi name and wifi password
        sprintf(cmd, "Setting.wifi.txt=\"%s\"\xFF\xFF\xFF",Param.WN);
        ESP_LOGD(TX_TASK_TAG, "Setting.wifi.txt=%s", Param.WN);
        NextionSendData(TX_TASK_TAG, cmd);

        sprintf(cmd, "Setting.pwd.txt=\"%s\"\xFF\xFF\xFF",Param.WP);
        ESP_LOGD(TX_TASK_TAG, "Setting.pwd.txt=%s", Param.WP);
        NextionSendData(TX_TASK_TAG, cmd);


        if(device.LED1State){
            NextionSendData(TX_TASK_TAG, "Control.t1.txt=\"ON\"\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.t1.pco=2016\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt0.val=1\xFF\xFF\xFF");
            //DeviceSetLevel(FAN1, ON);

        }else if(!device.LED1State){
            NextionSendData(TX_TASK_TAG, "Control.t1.txt=\"OFF\"\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.t1.pco=63488\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.bt0.val=0\xFF\xFF\xFF");	
            //DeviceSetLevel(FAN1, OFF);
        }

        if(device.LED2State){
            NextionSendData(TX_TASK_TAG, "Control.t4.txt=\"ON\"\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.t4.pco=2016\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt1.val=1\xFF\xFF\xFF");
            //DeviceSetLevel(FAN2, ON);

        }else if(!device.LED2State){
            NextionSendData(TX_TASK_TAG, "Control.t4.txt=\"OFF\"\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.t4.pco=63488\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt1.val=0\xFF\xFF\xFF");
            //DeviceSetLevel(FAN2, OFF);
        }

        if(device.Fan1State){
            NextionSendData(TX_TASK_TAG, "Control.t6.txt=\"ON\"\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.t6.pco=2016\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt2.val=1\xFF\xFF\xFF");
            //DeviceSetLevel(LED1, ON);
        }else if(!device.Fan1State){
            NextionSendData(TX_TASK_TAG, "Control.t6.txt=\"OFF\"\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.t6.pco=63488\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt2.val=0\xFF\xFF\xFF");
            //DeviceSetLevel(LED1, OFF);
        }

        if(device.Fan2State){
            NextionSendData(TX_TASK_TAG, "Control.t8.txt=\"ON\"\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.t8.pco=2016\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt3.val=1\xFF\xFF\xFF");
            //DeviceSetLevel(LED2, ON);
        }else if(!device.Fan2State){
            NextionSendData(TX_TASK_TAG, "Control.t8.txt=\"OFF\"\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.t8.pco=63488\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt3.val=0\xFF\xFF\xFF");
            //DeviceSetLevel(LED2, OFF);
        }

        if (device.Fan1State && device.Fan2State && device.LED1State && device.LED2State) {
            NextionSendData(TX_TASK_TAG, "Control.t9.txt=\"ON\"\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.t9.pco=2016\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt4.val=1\xFF\xFF\xFF");	
            // DeviceSetLevel(FAN1, ON);
            // DeviceSetLevel(FAN2, ON);
            // DeviceSetLevel(LED1, ON);
            // DeviceSetLevel(LED2, ON);
        } else if (! (device.Fan1State) ) {
            NextionSendData(TX_TASK_TAG, "Control.t9.txt=\"OFF\"\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.t9.pco=63488\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt4.val=0\xFF\xFF\xFF");
            //DeviceSetLevel(FAN1, OFF);
        }else if (! (device.Fan2State) ) {
            NextionSendData(TX_TASK_TAG, "Control.t9.txt=\"OFF\"\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.t9.pco=63488\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt4.val=0\xFF\xFF\xFF");
            //DeviceSetLevel(FAN2, OFF);
        }else if (! (device.LED1State) ) {
            NextionSendData(TX_TASK_TAG, "Control.t9.txt=\"OFF\"\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.t9.pco=63488\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt4.val=0\xFF\xFF\xFF");
            //DeviceSetLevel(LED1, OFF);
        }else if (! (device.LED2State) ) {
            NextionSendData(TX_TASK_TAG, "Control.t9.txt=\"OFF\"\xFF\xFF\xFF");
            NextionSendData(TX_TASK_TAG, "Control.t9.pco=63488\xFF\xFF\xFF");	
            NextionSendData(TX_TASK_TAG, "Control.bt4.val=0\xFF\xFF\xFF");
            //DeviceSetLevel(LED2, OFF);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); //Transmit every 10 seconds
    }
}

void NextionRXTask(void* arg)
{
    ESP_LOGD(RX_TASK_TAG,"NEXTION RX TASK");
    uint8_t* data = (uint8_t*) malloc(RX_BUFFER+1);
    char *dstream = malloc(RX_BUFFER+1);
    while (1) {
        ESP_LOGD(RX_TASK_TAG, "Free memory: %d bytes", esp_get_free_heap_size());
        memset(dstream,0,sizeof(malloc(RX_BUFFER+1)));
        const int rxBytes = uart_read_bytes(UART_HMI, data, RX_BUFFER, 500 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            snprintf(dstream, RX_BUFFER+1, "%s", data);
        }
        ESP_LOGD(RX_TASK_TAG, "data recv %s", dstream);

        cJSON *sub;
        sub=cJSON_Parse(dstream);

        cJSON *body =cJSON_GetObjectItem(sub,"body");
        char *value_body;
        //char *value_type_cmd;
        if(body){
            value_body=cJSON_GetObjectItem(sub,"body")->valuestring;
            ESP_LOGD(RX_TASK_TAG, "body is %s",value_body);
        }else{
            value_body="";
        }
        cJSON *command =cJSON_GetObjectItem(sub,"command");
        if(command){
            char *value_type_cmd =cJSON_GetObjectItem(sub,"command")->valuestring;
            ESP_LOGI(RX_TASK_TAG, "command is %s",value_type_cmd);
            ParseCmd(value_type_cmd, value_body);
        }
        //if(ParseCmd(value_type_cmd, value_body)==0)continue;
        // free(value_body);
        // free(value_type_cmd);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    free(data);
    free(dstream);
}