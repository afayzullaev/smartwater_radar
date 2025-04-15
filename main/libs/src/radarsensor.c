#include "modbussettings.h"
#include "radarsensor.h"

static const char *TAG = "RADAR_SENSOR";

float get_velocity(void)
{
    main_app_t appdata = {0};
    mb_param_request_t command = {
        .slave_addr = RADAR_ADDR,
        .reg_start = RADAR_REG_START,
        .reg_size = RADAR_REG_SIZE,
        .command =  RADAR_FC
    };
    uint16_t buff[2] = {0};
    uint32_t angle[2] = {0};
    char text[15];

    command.reg_start = RADAR_REG_START;
    command.reg_size = RADAR_REG_SIZE;
    esp_err_t err = mbc_master_send_request(&command,buff);
    if (err != ESP_OK) return 0; // ?
    
    sprintf(text, " %.02f m/s", ((float)buff[1])/((float)1000.0));
    if(buff[0] == 0){
        ESP_LOGI("MODBUS", "FORWARD, SPEED %.02f", ((float)buff[1])/((float)1000.0));
        write_text(" FORWARD",0);
        write_text("           ",1);
        write_text(text,1);
        appdata.sensor.velocity = ((float)buff[1])/((float)1000.0);
    }else{
        ESP_LOGI("MODBUS", "REVERSE, SPEED %.02f", ((float)buff[1])/((float)1000.0));
        write_text(" REVERSE",0);
        write_text("           ",1);
        write_text(text,1);
        appdata.sensor.velocity = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(500));
    command.reg_start = RADAR_ANGLE_REG_START;
    command.reg_size = RADAR_ANGLE_REG_SIZE;
    mbc_master_send_request(&command,angle);
    ESP_LOGI("MODBUS", "ANGLE %ld %ld", angle[0], angle[1]);
    return appdata.sensor.velocity; 
}

