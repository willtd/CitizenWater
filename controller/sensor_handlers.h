/*
 * sensor_handler.h
 */


#ifndef __SENSOR_HANDLERS_H__

#define __SENSOR_HANDLERS_H__

#include <stdlib.h>
#include "handler.h"

/* handle_calibrate - handle a json file and use its data to calibrate device sensors
 *
 * Param: 
 * 1) char * sensorType - Name of the sensor that will be calibrated (ph, conductivity, dissolved oxygen, and Oxygen Reduction Potential)
 * 2) char *sensorBrand - Name of the sensorBrand. The one that will apply for this project is "atlas-scientific"
 * 3) struct cJSON * cjsonParams - json file that conprised of 
 * parameters to use for calibration.
 * 4) struct cJSON * ret_obj - return json file
 *
 * Returns 0 if successful or -1 if fail
 */
int handle_calibrate(char * sensorType, char * sensorBrand, struct cJSON * cjsonParams, struct cJSON * ret_obj);

/* handle_take_reading - handle a json file and use its data to read device sensors
 *
 * Param:
 * 1) char * sensorType - Name of the sensor that will be calibrated (ph, conductivity, dissolved oxygen, and Oxygen Reduction Potential)
 * 2) char *sensorBrand - Name of the sensorBrand. The one that will apply for this project is "atlas-scientific"
 * 3) struct cJSON * ret_obj - return json file
 *
 * Returns 0 if success or -1 if fail
 */
int handle_take_reading(char * sensorType, char * sensorBrand, struct cJSON * cjsonParams, struct cJSON * ret_obj);

#endif
