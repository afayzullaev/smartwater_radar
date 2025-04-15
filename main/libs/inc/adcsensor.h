#ifndef __ADCSENSOR_H__
#define __ADCSENSOR_H__

#include "main.h"

//Custom board
// #define BATTERY_CHANNEL     ADC_CHANNEL_0
// #define SOLAR_CHANNEL       ADC_CHANNEL_7

//Lilygo
#define BATTERY_CHANNEL     ADC_CHANNEL_7
#define SOLAR_CHANNEL       ADC_CHANNEL_0

bms_t get_bms_data(void);

#endif 