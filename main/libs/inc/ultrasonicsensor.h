#ifndef __ULTRASONICSENSOR_H__
#define __ULTRASONICSENSOR_H__

#include "main.h"

//Ultrasonic sensor parameters
#define US_PORT_NUM             UART_NUM_2
#define US_TXD                  GPIO_NUM_19
#define US_RXD                  GPIO_NUM_18
#define US_RX_BUF_SIZE          1024

void UsUartInit(gpio_num_t tx, gpio_num_t rx);
void UsUartDeinit(void);
float get_distance(void);
void us_gpio_init(void);
void us_on(void);
void us_off(void);

#endif 