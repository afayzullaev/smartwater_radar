#include "modbussettings.h"
#include "pressuresensor.h"
#include "sdcard.h"

static const char *TAG = "PRESSURE_SENSOR";

static void change_unit(uint8_t unit)
{
    mb_param_request_t command = {
        .slave_addr = P_ADDR,
        .reg_start = 0.00,
        .reg_size = P_REG_SIZE,
        .command =  0x06
    };
}

float get_pressure(void)
{
    mb_param_request_t command = {
        .slave_addr = P_ADDR,
        .reg_start = P_REG_START,
        .reg_size = P_REG_SIZE,
        .command =  P_FC
    };
    uint16_t buff[3];

    esp_err_t err = mbc_master_send_request(&command,&buff);
    if (err != ESP_OK){
        system_logging(PATH_SYSTEM,"|ERROR| => failed to get pressure data");
        return 0.0;
    }
    ESP_LOGI(TAG, "unit %d", buff[0]);
    ESP_LOGI(TAG, "decimal point offset %d", buff[1]);
    ESP_LOGI(TAG, "measurment %d", buff[2]);

    ESP_LOGI(TAG, "Pressure %f KPa", (float)buff[2]*9.80665 * 0.01);
    
    vTaskDelay(pdMS_TO_TICKS(500));
    return (float)buff[2]*9.80665 * 0.01;     
}

float get_level(float pressure, uint16_t density)
{
    return ((pressure*1000.0) / ((float)density * 9.8))*100; 
}