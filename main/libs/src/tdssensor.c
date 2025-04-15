#include "modbussettings.h"
#include "tdssensor.h"
#include "sdcard.h"

static const char *TAG = "TDS_SENSOR";

float get_tds(void)
{
    mb_param_request_t command = {
        .slave_addr = TDS_ADDR,
        .reg_start = TDS_REG_START,
        .reg_size = TDS_REG_SIZE,
        .command =  TDS_FC
    };
    uint16_t buff;

    esp_err_t err = mbc_master_send_request(&command,&buff);
    if (err != ESP_OK){
        system_logging(PATH_SYSTEM,"|ERROR| => failed to get tds data");
        return 0.0;
    } 
    ESP_LOGI(TAG, " %.02f ppm", (float)buff);

    vTaskDelay(pdMS_TO_TICKS(500));
    return (float)buff; 
}

float get_salinity(void)
{
    mb_param_request_t command = {
        .slave_addr = TDS_ADDR,
        .reg_start = SALINITY_REG_START,
        .reg_size = TDS_REG_SIZE,
        .command =  TDS_FC
    };
    uint16_t buff;

    esp_err_t err = mbc_master_send_request(&command,&buff);
    if (err != ESP_OK){
        system_logging(PATH_SYSTEM,"|ERROR| => failed to get tds data");
        return 0.0;
    } 
    ESP_LOGI(TAG, " %.02f ppm", (float)buff);

    vTaskDelay(pdMS_TO_TICKS(500));
    return (float)buff; 
}

float get_tds_temperature(void)
{
    mb_param_request_t command = {
        .slave_addr = TDS_ADDR,
        .reg_start = TDS_TEMPERATURE_REG_START,
        .reg_size = TDS_REG_SIZE,
        .command =  TDS_FC
    };
    uint16_t buff;

    esp_err_t err = mbc_master_send_request(&command,&buff);
    if (err != ESP_OK) return 0.0;
    ESP_LOGI(TAG, " %.02f C", ((float)buff / 10.0));

    vTaskDelay(pdMS_TO_TICKS(500));
    return ((float)buff / 10.0); 
}

