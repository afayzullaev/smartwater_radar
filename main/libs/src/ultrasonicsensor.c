#include "ultrasonicsensor.h"
#include "sdcard.h"

static const char *TAG = "ULTRASONIC_SENSOR";

void us_gpio_init(void)
{
    gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
}

void us_on(void)
{
    gpio_set_level(GPIO_NUM_32, 1);
}

void us_off(void)
{
    gpio_set_level(GPIO_NUM_32, 0);
}

void UsUartInit(gpio_num_t tx, gpio_num_t rx){
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits  = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(US_PORT_NUM, &uart_config);
    uart_set_pin(US_PORT_NUM, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(US_PORT_NUM, US_RX_BUF_SIZE, 0, 0, NULL, 0);
	ESP_LOGI("US", "Ultasonic sensor driver installed");
}

void UsUartDeinit(void){
    uart_driver_delete(US_PORT_NUM);
}

float get_distance(void)
{
    uint8_t data[4] = {0};
    float distance = 0;
    uart_read_bytes(US_PORT_NUM, data, 4, pdMS_TO_TICKS(5000));
    if(data[0] == 0xFF){
        distance = data[1] << 8 | data[2];
        if (distance > 300 && distance < 8000) {
            ESP_LOGI(TAG, "RELIABLE VALUE");
            distance /= 10;                        
            ESP_LOGI(TAG, "distance = %f", distance);
            return distance;				
        }else{
            ESP_LOGI(TAG, "CAN'T GET RELIABLE VALUE");
            // system_logging(PATH_SYSTEM,"|ERROR| => CAN'T GET RELIABLE VALUE FROM ULTRASONIC LEVEL SENSOR");
            return 0.0;
        }
    }else{
        return 0.0;
    }
}
