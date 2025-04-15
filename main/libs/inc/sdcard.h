#ifndef __SDCARD_H__
#define __SDCARD_H__

#include "main.h"

#define PIN_CS          GPIO_NUM_13
#define PIN_MOSI        GPIO_NUM_15
#define PIN_CLK         GPIO_NUM_14
#define PIN_MISO        GPIO_NUM_2

#define PATH_DATA       "/store/data.txt"
#define PATH_SYSTEM     "/store/system.txt"

void data_logging(char *path, char *header,char *content);
void system_logging(char *path,char *content);
#endif 