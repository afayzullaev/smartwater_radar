#ifndef __RADARSENSOR_H__
#define __RADARSENSOR_H__

#include "main.h"

//Flowmeter parameters
#define RADAR_ADDR              0x34
#define RADAR_REG_START         0x01F6
#define RADAR_REG_SIZE          0x02
#define RADAR_FC                0x03
#define RADAR_ANGLE_REG_START   0x0A
#define RADAR_ANGLE_REG_SIZE    0x01

float get_velocity(void);


#endif 