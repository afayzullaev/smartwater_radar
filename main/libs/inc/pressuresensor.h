#ifndef __PRESSURESENSOR_H__
#define __PRESSURESENSOR_H__

#include "main.h"

//Pressure sensor units
#define MPa                               0x0000
#define kPa                               0x0000
#define Pa                                0x0000
#define bar                               0x0000
#define kgcm2                             0x0000

//Pressure sensor adresses
#define P_ADDR                            0x01
#define P_REG_START                       0x02
#define P_REG_SIZE                        0x03
#define P_FC                              0x03


float get_pressure(void);
float get_level(float pressure, uint16_t density);

#endif 