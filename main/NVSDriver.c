#include "NVSDriver.h"
#include "nvs.h"
#include "esp_log.h"
#include "string.h"

nvs_handle my_handle;

static const char* TAG = "NVS Driver";

void NVSDriverInit(){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
}

void NVSDriverOpen(const char* paramname)
{
    nvs_open(paramname, NVS_READWRITE, &my_handle);
}

void NVSDriverClose()
{
    nvs_close(my_handle);
}

esp_err_t NVSDriverCommit()
{
    esp_err_t err = nvs_commit(my_handle);
    return err;
}

bool NVSDriverRead(const char* paramname, char**out)
{
    size_t required_size;
    esp_err_t err = nvs_get_str(my_handle, paramname, NULL, &required_size);
    if(err==ESP_OK)
    {
        *out =(char*) malloc(required_size);
        if(!out) {ESP_LOGE(TAG, "malloc fail"); return false;}
        nvs_get_str(my_handle, paramname, *out, &required_size);
        ESP_LOGI(TAG, "load param=%s val=%s", paramname, *out);
        return true;
    }
    ESP_LOGI(TAG, "load param=%s err=%s", paramname, esp_err_to_name(err));
    return false;
}

esp_err_t NVSDriverWrite(const char* paramname, char *val)
{
    esp_err_t err = nvs_set_str(my_handle, paramname, val);
    if(err==ESP_OK)
    {
        ESP_LOGI(TAG, "save param=%s val=%s", paramname, val);
    } else
    {
        ESP_LOGI(TAG, "save param=%s err=%s", paramname, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverEraseKey(const char* paramname)
{
    esp_err_t err = nvs_erase_key(my_handle, paramname);
    if(err==ESP_OK)
    {
        ESP_LOGI(TAG, "erase param=%s", paramname);
    } else
    {
        ESP_LOGI(TAG, "erase param=%s err=%s", paramname, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverEraseAll()
{
    esp_err_t err = nvs_erase_all(my_handle);
    if(err==ESP_OK)
    {
        ESP_LOGI(TAG, "erase all");
    } else
    {
        ESP_LOGI(TAG, "erase all err=%s", esp_err_to_name(err));
    }
    return err;
}