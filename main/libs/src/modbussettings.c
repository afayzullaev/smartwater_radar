#include "modbussettings.h"

static const char *TAG = "MODBUS";

// Modbus master initialization
esp_err_t master_init(gpio_num_t tx, gpio_num_t rx, gpio_num_t rts)
{
    vTaskDelay(pdMS_TO_TICKS(500));
    // Initialize and start Modbus controller
    mb_communication_info_t comm = {
        .port = MB_PORT_NUM,
        .mode = MB_MODE_RTU,
        .baudrate = MB_DEV_SPEED,
        .parity = MB_PARITY_NONE
    };
    void* master_handler = NULL;

    esp_err_t err = mbc_master_init(MB_PORT_SERIAL_MASTER, &master_handler);
    MB_RETURN_ON_FALSE((master_handler != NULL), ESP_ERR_INVALID_STATE, TAG,"mb controller initialization fail.");
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE, TAG,"mb controller initialization fail, returns(0x%x).", (int)err);
    err = mbc_master_setup((void*)&comm);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE, TAG,"mb controller setup fail, returns(0x%x).", (int)err);

    // Set UART pin numbers

    err = uart_set_pin(MB_PORT_NUM, tx, rx, rts, UART_PIN_NO_CHANGE);

    err = mbc_master_start();
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE, TAG,"mb controller start fail, returned (0x%x).", (int)err);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE, TAG,"mb serial set pin failure, uart_set_pin() returned (0x%x).", (int)err);

    // Set driver mode to Half Duplex
    err = uart_set_mode(MB_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX);
    MB_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE, TAG,"mb serial set mode failure, uart_set_mode() returned (0x%x).", (int)err);

    vTaskDelay(5);
    return err;
}

void MasterDeinit(void){
    mbc_master_destroy();
}

/**
 * RS485 power pin control
*/
void modbus_pwr_on(void)
{
    gpio_set_direction(MB_PWR, GPIO_MODE_OUTPUT);
    gpio_set_level(MB_PWR, 1);
}

void modbus_pwr_off(void)
{
    gpio_set_direction(MB_PWR, GPIO_MODE_OUTPUT);
    gpio_set_level(MB_PWR, 0);
}