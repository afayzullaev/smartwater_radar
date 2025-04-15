#include "esp_log.h"
#include "main.h"
#include "sensor.h"
#include "ultrasonicsensor.h"
#include "internet.h"
#include "modemrelated.h" 
#include "esp_timer.h"

static const char *TAG = "MAIN_EVENT";

QueueHandle_t eventqueue = NULL;


void dispatcher_task(void *pvParameters)
{
    ESP_LOGI("DISPATCHER", "DISPATCHER TASK STARTED");
    main_app_t mainobj;
    mainobj.event = REQUEST_TO_SENSORS;
    xQueueSend(eventqueue, &mainobj, pdMS_TO_TICKS(10000));
    while (1)
    {
        if (xQueueReceive(eventqueue, &mainobj, portMAX_DELAY))
        {
            switch (mainobj.event)
            {
            case REQUEST_TO_SENSORS:
                ESP_LOGI(TAG, "REQUEST_TO_SENSORS");
                sensors_dispatcher(eventqueue);
                break;
            
            case GOT_DATA_FROM_SENSORS:
                ESP_LOGI(TAG, "GOT_DATA_FROM_SENSORS");
                send_data_to_server(mainobj);
                break;
            
            default:
                break;
            }
        }
    }
}

void app_main(void)
{
    // Initialization of device peripheral and objects
    eventqueue = xQueueCreate(1,sizeof(main_app_t));
    // modbus_pwr_on();
    gpio_config_t io_conf = {};
    io_conf.intr_type=GPIO_INTR_DISABLE;
    io_conf.mode=GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask= 1ULL << GPIO_NUM_12 | 1ULL << GPIO_NUM_14;
    gpio_config(&io_conf);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(GPIO_NUM_12,1);

    gpio_set_level(GPIO_NUM_14, 0);
    a7670_init();
    vTaskDelay(10);
    // display_init();
    // clear_text();
    xTaskCreate(dispatcher_task, "dispatcher_task", 32768, NULL, 6, NULL);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


