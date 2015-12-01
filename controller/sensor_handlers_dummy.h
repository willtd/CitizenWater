

#ifndef SENSOR_HANDLERS__

#define SENSOR_HANDLERS__

#include <stdlib.h>
#include "../cJSON/cJSON.h"

int handle_calibrate(char * sensorType, char * sensorBrand, struct cJSON * cjsonParams);
int handle_take_reading(char * sensorType, char * sensorBrand);

#endif


