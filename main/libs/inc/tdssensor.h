#ifndef __TDSSENSOR_H__
#define __TDSSENSOR_H__

#include "main.h"

//tds sensor parameters
#define TDS_ADDR                            0x04
#define TDS_REG_START                       0x02
#define SALINITY_REG_START                  0x06
#define TDS_TEMPERATURE_REG_START           0x08
#define TDS_REG_SIZE                        0x02
#define TDS_FC                              0x04

float get_tds(void);
float get_salinity(void);
float get_tds_temperature(void);

#endif 