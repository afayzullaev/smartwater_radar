#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "main.h"

#define MAX_SAMPLES             10


//Sensors
void sensors_dispatcher(QueueHandle_t queue);

#endif 