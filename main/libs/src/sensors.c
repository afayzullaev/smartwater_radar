#include "sensor.h"
#include "modbussettings.h"
#include "ultrasonicsensor.h"
#include "radarsensor.h"
#include "tdssensor.h"
#include "pressuresensor.h"
#include "adcsensor.h"
#include "statistics.h"

static const char *TAG = "SENSOR_DISPATCHER";

QueueHandle_t sensorqueue = NULL;

static sensor_t average_sensor_value(int size)
{
    sensor_t sensor;
    double velocity[size], distance[size];
    
    gpio_set_level(GPIO_NUM_14, 1);
    for (size_t i = 0; i < size; i++)
    {
        ESP_ERROR_CHECK(master_init(MB_TX, MB_RX, MB_RTS));
        vTaskDelay(pdMS_TO_TICKS(100));
        velocity[i] = get_velocity();
        MasterDeinit();
        vTaskDelay(pdMS_TO_TICKS(100));
        UsUartInit(US_TXD, US_RXD);
        distance[i] = get_distance();
        UsUartDeinit();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    gpio_set_level(GPIO_NUM_14, 0);
    sensor.velocity = mean_distance(velocity, size);
    sensor.measured_distance = mean_distance(distance, size);
    ESP_LOGI(TAG, "Average velocity: %fl", sensor.velocity);
    ESP_LOGI(TAG, "Average distance: %fl", sensor.measured_distance);
    return sensor;
}


static float average_mbsensor_value(float (*f)(void), char* type, int sample_size)
{
    float sensor_value = 0, number_of_samples = 0;
    for (size_t i = 0; i < sample_size; i++)
    {
        ESP_ERROR_CHECK(master_init(MB_TX, MB_RX, MB_RTS));
        number_of_samples++;
        sensor_value += f();
        MasterDeinit();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    if(number_of_samples == 0) return 0;
    sensor_value = sensor_value / number_of_samples;
    ESP_LOGI(TAG, "Average %s: %.02f ", type, sensor_value);
    return sensor_value;
}

void sensors_dispatcher(QueueHandle_t queue)
{
    sensorqueue = queue;
    main_app_t mainobj;
    
    mainobj.sensor = average_sensor_value(MAX_SAMPLES);
    mainobj.bms = get_bms_data();
    mainobj.event = GOT_DATA_FROM_SENSORS;

    vTaskDelay(pdMS_TO_TICKS(100));
    xQueueSend(sensorqueue, &mainobj, pdMS_TO_TICKS(10000));
}



