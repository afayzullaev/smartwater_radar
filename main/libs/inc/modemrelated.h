#ifndef __A9GRELATED_H__
#define __A9GRELATED_H__
#include "main.h"
#include "driver/uart.h"
#include "esp_modem_api.h"

#define MODEM_UART_NUM              UART_NUM_1

#define A7670_RESET                 GPIO_NUM_5
#define A7670_PWKEY                 GPIO_NUM_4

#define GSM_RX                      GPIO_NUM_27
#define GSM_TX                      GPIO_NUM_26
#define APN                         "internet.beeline.uz"
#define MAX_RETRY                   5

#define set_a7670_rst() gpio_set_level(A7670_RESET,1)
#define clear_a7670_rst() gpio_set_level(A7670_RESET,0)

#define set_a7670_pwkey() gpio_set_level(A7670_PWKEY,1)
#define clear_a7670_pwkey() gpio_set_level(A7670_PWKEY,0)


void a7670_init(void);
void a7670_turnon(void);
void a7670_turnoff(void);

uint8_t check_signal_quality(esp_modem_dce_t *dce);
gps_t get_gnss_data(esp_modem_dce_t *dce);
gps_t get_NMEA_gnss_data(esp_modem_dce_t *dce);
#endif 