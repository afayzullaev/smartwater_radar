#ifndef __MODBUSSETTINGS_H__
#define __MODBUSSETTINGS_H__

#include "main.h"

//Modbus parameters
#define MB_PORT_NUM             UART_NUM_2 // Number of UART port used for Modbus connection
#define MB_DEV_SPEED            9600  // The communication speed of the UART

#define MB_PWR                  33
#define MB_TX                   GPIO_NUM_22
#define MB_RX                   GPIO_NUM_23
// #define MB_RTS                  18
#define MB_RTS                  GPIO_NUM_21
#define MB_CTS                  (-1)

//Modbus related
esp_err_t master_init(gpio_num_t tx, gpio_num_t rx, gpio_num_t rts);
void modbus_pwr_on(void);
void modbus_pwr_off(void);
void MasterDeinit(void);

#endif 