#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mbcontroller.h"
#include "ssd1306.h"
#include "driver/gpio.h"
#include "i2cdev.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "mbcontroller.h"
#include "cJSON.h"


#define GOT_IP_BIT              BIT0
#define LOST_IP_BIT             BIT1

typedef enum{
    REQUEST_TO_SENSORS = 0,
    GOT_DATA_FROM_SENSORS,
}main_event_t;

typedef struct{
    float          tds;
    float          salinity;
    float          pressure;
    float          temperature;
    float          measured_distance;
    float          level;
    float          velocity;
}sensor_t;

typedef struct{
    float          bat_voltage;
    bool           charging;
}bms_t;

typedef struct{
    char           latitude[16];
    char           longitude[16];
}gps_t;

typedef struct{
    main_event_t    event;
    bms_t           bms;
    sensor_t        sensor;
    gps_t           gnss;
    char            imei[16];
    uint8_t         signal_quality;
}main_app_t;


//Display parameters
#define OLED_SDA                GPIO_NUM_26
#define OLED_SCL                GPIO_NUM_27

// #define SERVER_URL              "https://api.smartwaterdegree.uz/consumption"
// #define SERVER_URL              "https://api.simsimdevices.uz/api/v1/devices/water-data/"
#define SERVER_URL              "https://farmerapi.simsimdevices.uz/api/v1/devices/create-device-data/"

/**
 * Function declarations
*/
// OLED initialization
void display_init(void);
void write_text(char* buff, uint8_t row);
void clear_text(void);



#endif 