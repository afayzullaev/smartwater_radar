#include "lowpowerfunctions.h"
#include "esp_sleep.h"
#include "modemrelated.h"

static const char *TAG = "SLEEPMODE";

void sleep_device(void)
{
    ESP_LOGI(TAG, "sleeping device");
    a7670_turnoff();
    esp_sleep_enable_timer_wakeup(SLEEP_TIME);
    vTaskDelay(pdMS_TO_TICKS(5000));
    esp_deep_sleep_start();
}