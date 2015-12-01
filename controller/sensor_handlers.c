/*
 * sensor_handlers.c
 *
 *  Contains the logic of intrepreting JSON files to calibrate
 *  and read device sensors.
 */

#include "sensor_handlers.h"
#include "../shared/log.h"
#include "../shared/util.h"
#include "../atlsci-sensors/sensors.h"
#include "../database/database.h"

#include <strings.h>
#include <stdio.h>

#define ATLSCI_PH_STRING "pH"
#define ATLSCI_CONDUCTIVITY_STRING "Conductivity"
#define ATLSCI_DO_STRING "Dissolved Oxygen"
#define ATLSCI_ORP_STRING "Oxygen Reduction Potential"

#define READINGS_DB "home/citizenwater/citizenwater.db"

static int atlsci_string_to_int(char * sensorType)
{
    if (!strcasecmp(sensorType, ATLSCI_PH_STRING))
    {
        return ATLSCI_PH;
    }
    else if (!strcasecmp(sensorType, ATLSCI_CONDUCTIVITY_STRING))
    {
        return ATLSCI_CONDUCTIVITY;
    }
    else if (!strcasecmp(sensorType, ATLSCI_DO_STRING))
    {
        return ATLSCI_DO;
    }
    else if (!strcasecmp(sensorType, ATLSCI_ORP_STRING))
    {
        return ATLSCI_ORP;
    }
    else
    {
        return -1;
    }
}

static int handle_ph_calibrate(struct cJSON * cjsonParams, struct cJSON * ret_obj)
{
    cJSON *json_param = cjsonParams->child;
    atlsci_cal_param_t action;
    atlsci_cal_ret_t retVal = {0};

    int ret = 0;
    float val = 0;
    char msgBuffer[100] = {0};

    if(cJSON_GetObjectItem(cjsonParams, "PH_CALIBRATION_CLEAR") != NULL)
    {
        action.cal_ph = PH_CALIBRATION_CLEAR;
        retVal = atlsci_cal(ATLSCI_PH, action, 0);
        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "PH sensor calibration is reset to factory");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "PH sensor calibration failed to reset to factory");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    
    else if(cJSON_GetObjectItem(cjsonParams, "PH_CALIBRATION_QUERY") != NULL)
    {
        action.cal_ph = PH_CALIBRATION_QUERY;
        retVal = atlsci_cal(ATLSCI_PH, action, 0);

        if (retVal.success == 1)
        {
            DEBUG("retVal.query : %s", retVal.query);
            if(!strcasecmp(retVal.query, "?CAL,0"))
                strcpy(msgBuffer, "PH sensor is not calibrated");
            else if(!strcasecmp(retVal.query, "?CAL,1"))
                strcpy(msgBuffer, "PH sensor calibrated to use single-point calibration");
            else if(!strcasecmp(retVal.query, "?CAL,2"))
                strcpy(msgBuffer, "PH sensor calibrated to use double-point calibration");
            else if(!strcasecmp(retVal.query, "?CAL,3"))
                strcpy(msgBuffer, "PH sensor calibrated to use triple-point calibration");
            else 
                strcpy(msgBuffer, retVal.query);

            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
            free(retVal.query);
        }
        else
        {
            strcpy(msgBuffer, "PH sensor calibration failed to query calibration");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;

        }
    }
    else if(cJSON_GetObjectItem(cjsonParams, "PH_LOW_CALIBRATION_POINT") != NULL)
    {
        action.cal_ph = PH_LOW_CALIBRATION_POINT;
        val = (float) json_param->valuedouble;
        retVal = atlsci_cal(ATLSCI_PH, action, val);

        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated pH low calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate pH low calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    else if(cJSON_GetObjectItem(cjsonParams, "PH_MID_CALIBRATION_POINT") != NULL)
    {
        action.cal_ph = PH_MID_CALIBRATION_POINT;
        val = (float) json_param->valuedouble;
        retVal = atlsci_cal(ATLSCI_PH, action, val);

        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated pH mid calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate pH mid calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    else if(cJSON_GetObjectItem(cjsonParams, "PH_HIGH_CALIBRATION_POINT") != NULL)
    {
        action.cal_ph = PH_HIGH_CALIBRATION_POINT;
        val = (float) json_param->valuedouble;
        retVal = atlsci_cal(ATLSCI_PH, action, val);

        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated pH high calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate pH high calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    else if(cJSON_GetObjectItem(cjsonParams, "PH_TEMPERATURE_COMPENSATION") != NULL)
    {
        atlsci_temp_comp_param set_temp = SET_TEMPERATURE;
        atlsci_temp_comp_ret_t temp_ph = {0};
        val = (float) json_param->valuedouble;
        temp_ph = atlsci_temp_comp(ATLSCI_PH, set_temp, val);

        if(temp_ph.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated pH temperature");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate pH temperature");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    else 
        ret = -1;

    return ret;

}

static int handle_do_calibrate(struct cJSON *cjsonParams, struct cJSON * ret_obj)
{
    cJSON *json_param = cjsonParams->child;
    atlsci_cal_param_t action;
    atlsci_cal_ret_t retVal = {0};

    int ret = 0;
    char msgBuffer[100] = {0};

    if(cJSON_GetObjectItem(cjsonParams, "DO_CALIBRATION_CLEAR") != NULL)
    {
        action.cal_do = DO_CALIBRATION_CLEAR;
        retVal = atlsci_cal(ATLSCI_DO, action, 0);
        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "DO sensor calibration is reset to factory");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "DO sensor calibration failed to reset to factory");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    
    else if(cJSON_GetObjectItem(cjsonParams, "DO_CALIBRATION_QUERY") != NULL)
    {
        action.cal_do = DO_CALIBRATION_QUERY;
        retVal = atlsci_cal(ATLSCI_DO, action, 0);
        if (retVal.success == 1)
        {
            INFO("retVal.query : %s", retVal.query);
            if(!strcasecmp(retVal.query, "?CAL,0"))
                strcpy(msgBuffer, "DO sensor is not calibrated");
            else if(!strcasecmp(retVal.query, "?CAL,1"))
                strcpy(msgBuffer, "DO sensor calibrated to use single-point calibration");
            else if(!strcasecmp(retVal.query, "?CAL,2"))
                strcpy(msgBuffer, "DO sensor calibrated to use double-point calibration");
            else 
                strcpy(msgBuffer, retVal.query);

            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
            free(retVal.query);
        }

        else
        {
            strcpy(msgBuffer, "DO sensor calibration failed to query calibration");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;

        }
    }

    else if(cJSON_GetObjectItem(cjsonParams, "DO_ATMOSPHERIC_CALIBRATION") != NULL)
    {
        action.cal_do = DO_ATMOSPHERIC_CALIBRATION;
        retVal = atlsci_cal(ATLSCI_DO, action, 0);
        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated atmospheric oxygen level");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate atmospheric oxygen level");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    else if(cJSON_GetObjectItem(cjsonParams, "DO_ZERO_LEVEL_CALIBRATION") != NULL)
    {
        action.cal_do = DO_ZERO_LEVEL_CALIBRATION;
        retVal = atlsci_cal(ATLSCI_DO, action, 0);
        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated 0 dissolved oxygen level");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate to 0 dissolved oxygen level");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    else if(cJSON_GetObjectItem(cjsonParams, "DO_TEMPERATURE_COMPENSATION") != NULL)
    {
        atlsci_temp_comp_param set_temp = SET_TEMPERATURE;
        atlsci_temp_comp_ret_t temp_do = {0};
        float val = (float) json_param->valuedouble;
        temp_do = atlsci_temp_comp(ATLSCI_DO, set_temp, val);

        if(temp_do.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated do temperature");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate do temperature");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }

    else
    {
        ret = -1;
    }

    return ret;
}

static int handle_orp_calibrate(struct cJSON *cjsonParams, struct cJSON * ret_obj)
{
    cJSON *json_param = cjsonParams->child;
    atlsci_cal_param_t action;
    atlsci_cal_ret_t retVal = {0};

    int ret = 0;
    char msgBuffer[100] = {0};

    if(cJSON_GetObjectItem(cjsonParams, "ORP_CALIBRATION_CLEAR") != NULL)
    {
        action.cal_orp = ORP_CALIBRATION_CLEAR;
        retVal = atlsci_cal(ATLSCI_ORP, action, 0);
        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "ORP sensor calibration is reset to factory");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "ORP sensor calibration failed to reset to factory");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    
    else if(cJSON_GetObjectItem(cjsonParams, "ORP_CALIBRATION_QUERY") != NULL)
    {
        action.cal_orp = ORP_CALIBRATION_QUERY;
        retVal = atlsci_cal(ATLSCI_ORP, action, 0);
        if (retVal.success == 1)
        {
            INFO("retVal.query : %s", retVal.query);
            if(!strcasecmp(retVal.query, "?CAL,0"))
                strcpy(msgBuffer, "ORP sensor is not calibrated");
            else if(!strcasecmp(retVal.query, "?CAL,1"))
                strcpy(msgBuffer, "ORP sensor calibrated to use single-point calibration");
            else 
                strcpy(msgBuffer, retVal.query);

            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
            free(retVal.query);
        }

        else
        {
            strcpy(msgBuffer, "ORP sensor calibration failed to query calibration");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;

        }
    }
    else if(cJSON_GetObjectItem(cjsonParams, "ORP_SINGLE_CALIBRATION_POINT") != NULL)
    {
        action.cal_orp = ORP_SINGLE_CALIBRATION_POINT;
        float val = (float) json_param->valuedouble;
        retVal = atlsci_cal(ATLSCI_ORP, action, val);

        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated ORP single calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate ORP single calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }

     }

    else
    {
        ret = -1;
    }

    return ret;
}

static int handle_conductivity_calibrate(struct cJSON * cjsonParams, struct cJSON * ret_obj)
{
    cJSON *json_param = cjsonParams->child;
    atlsci_cal_param_t action;
    atlsci_cal_ret_t retVal = {0};

    int ret = 0;
    char msgBuffer[100] = {0};

    if(cJSON_GetObjectItem(cjsonParams, "CONDUCTIVITY_CALIBRATION_CLEAR") != NULL)
    {
        action.cal_conductivity = CONDUCTIVITY_CALIBRATION_CLEAR;
        retVal = atlsci_cal(ATLSCI_CONDUCTIVITY, action, 0);

        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "CONDUCTIVITY sensor calibration is reset to factory");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "CONDUCTIVITY sensor calibration failed to reset to factory");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    
    else if(cJSON_GetObjectItem(cjsonParams, "CONDUCTIVITY_CALIBRATION_QUERY") != NULL)
    {
        action.cal_conductivity = CONDUCTIVITY_CALIBRATION_QUERY;
        retVal = atlsci_cal(ATLSCI_CONDUCTIVITY, action, 0);
        if (retVal.success == 1)
        {
            INFO("retVal.query : %s", retVal.query);
            if(!strcasecmp(retVal.query, "?CAL,0"))
                strcpy(msgBuffer, "CONDUCTIVITY sensor is not calibrated");
            else if(!strcasecmp(retVal.query, "?CAL,1"))
                strcpy(msgBuffer, "CONDUCTIVITY sensor calibrated to use single-point calibration");
            else if(!strcasecmp(retVal.query, "?CAL,2"))
                strcpy(msgBuffer, "CONDUCTIVITY sensor calibrated to use double-point calibration");
            else 
                strcpy(msgBuffer, retVal.query);

            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
            free(retVal.query);
        }

        else
        {
            strcpy(msgBuffer, "CONDUCTIVITY sensor calibration failed to query calibration");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;

        }
    }

    else if(cJSON_GetObjectItem(cjsonParams, "CONDUCTIVITY_DRY_CALIBRATION") != NULL)
    {
        action.cal_conductivity = CONDUCTIVITY_DRY_CALIBRATION;
        retVal = atlsci_cal(ATLSCI_CONDUCTIVITY, action, 0);
        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated conductivity dry calibration");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate conductivity dry calibration");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }

    else if(cJSON_GetObjectItem(cjsonParams, "CONDUCTIVITY_SINGLE_CALIBRATION_POINT") != NULL)
    {
        action.cal_conductivity = CONDUCTIVITY_SINGLE_CALIBRATION_POINT;
        float val = (float) json_param->valuedouble;
        retVal = atlsci_cal(ATLSCI_CONDUCTIVITY, action, val);

        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated conductivity single calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate conductivity single calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }

     }
    else if(cJSON_GetObjectItem(cjsonParams, "CONDUCTIVITY_LOW_DUAL_CALIBRATION_POINT") != NULL)
    {
        action.cal_conductivity = CONDUCTIVITY_LOW_DUAL_CALIBRATION_POINT;
        float val = (float) json_param->valuedouble;
        retVal = atlsci_cal(ATLSCI_CONDUCTIVITY, action, val);

        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated conductivity low calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate conductivity low calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }

     }
    else if(cJSON_GetObjectItem(cjsonParams, "CONDUCTIVITY_HIGH_DUAL_CALIBRATION_POINT") != NULL)
    {
        action.cal_conductivity = CONDUCTIVITY_HIGH_DUAL_CALIBRATION_POINT;
        float val = (float) json_param->valuedouble;
        retVal = atlsci_cal(ATLSCI_CONDUCTIVITY, action, val);

        if(retVal.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated conductivity high calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate conductivity high calibration point");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }

     }
    else if(cJSON_GetObjectItem(cjsonParams, "CONDUCTIVITY_TEMPERATURE_COMPENSATION") != NULL)
    {
        atlsci_temp_comp_param set_temp = SET_TEMPERATURE;
        atlsci_temp_comp_ret_t temp_do = {0};
        float val = (float) json_param->valuedouble;
        temp_do = atlsci_temp_comp(ATLSCI_CONDUCTIVITY, set_temp, val);

        if(temp_do.success == 1)
        {
            strcpy(msgBuffer, "Successfully calibrated conductivity temperature");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
        }
        else
        {
            strcpy(msgBuffer, "Failed to calibrate conductivity temperature");
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString(msgBuffer));
            ret = -1;
        }
    }
    else
    {
        ret = -1;
    }

    return ret;
}

/* 
 * Calibrate a sensor and store its result of success/fail 
 * and query to database. More info can be found under 
 * sensor_handlers.h
 */
static int handle_atlsci_calibrate(char * sensorType, struct cJSON * cjsonParams, struct cJSON * ret_obj)
{
    int ret = 0;
    int sensorTypeAsInt = atlsci_string_to_int(sensorType);

    DEBUG("Calibrate...: %s\n", sensorType);
    if(!strcasecmp(sensorType, ATLSCI_PH_STRING))
    {
        ret = handle_ph_calibrate(cjsonParams, ret_obj);
    }
    else if(!strcasecmp(sensorType, ATLSCI_DO_STRING))
    {
        ret = handle_do_calibrate(cjsonParams, ret_obj);
    }
    else if(!strcasecmp(sensorType, ATLSCI_ORP_STRING))
    {
        ret = handle_orp_calibrate(cjsonParams, ret_obj);
    }
    else if(!strcasecmp(sensorType, ATLSCI_CONDUCTIVITY_STRING))
    {
        ret = handle_conductivity_calibrate(cjsonParams, ret_obj);
    }
    else
        ret = -1;

    atlsci_sleep_ret_t sleep = atlsci_sleep(sensorTypeAsInt);

    return ret;
}

/* 
 * Read a sensor and store its data result to database.
 * More info can be found under sensor_handlers.h
 * 
 */
static int handle_atlsci_take_reading(char * sensorType, struct cJSON * ret_obj)
{
    int ret = 0;

    DEBUG("Reading...: %s\n", sensorType);

    if (sqlite_open(READINGS_DB) < 0)
    {
        ERROR("Failed to open database\n");
        cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString("Failed to open database"));
        ret = -1;
    } 
    else
    {
        int sensorTypeAsInt = atlsci_string_to_int(sensorType);

        if (sensorTypeAsInt != -1)
        {
            atlsci_read_ret_t readval = {0};

            readval = atlsci_read(sensorTypeAsInt);

            if (readval.success == 1)
            {
                char msgBuffer[100]= {0};
                sprintf(msgBuffer, "%s value: %s", sensorType, readval.data);
                cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "messages"), cJSON_CreateString(msgBuffer));
                sqlite_insert_reading(sensorType, get_unix_timestamp(), readval.data);
            }
            else
            {
                ERROR("Read from sensor failed\n");
                cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString("Read from sensor failed"));
                ret = -1;
            }
            atlsci_sleep_ret_t sleep = atlsci_sleep(sensorTypeAsInt);
        }
        else
        {
            DEBUG("Unknown sensor type: %s\n", sensorType);
            cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString("Unknown sensor"));
            ret = -1;
        }

        sqlite_close();	//close database
    }


    return ret;
}

int handle_calibrate(char * sensorType, char * sensorBrand, struct cJSON * cjsonParams, struct cJSON * ret_obj)
{
    int ret = 0;

    if (!strcasecmp(sensorBrand, "atlas-scientific"))
    {
        ret = handle_atlsci_calibrate(sensorType, cjsonParams, ret_obj);
    }
    else
    {
        DEBUG("Unknown sensor brand: %s", sensorBrand);
        cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString("Unknown sensor"));
        ret = -1;
    }

    return ret;
}

int handle_take_reading(char * sensorType, char * sensorBrand, struct cJSON * cjsonParams, struct cJSON * ret_obj)
{
    int ret = 0;

    if (!strcasecmp(sensorBrand, "atlas-scientific"))
    {
        ret = handle_atlsci_take_reading(sensorType, ret_obj);
    }
    else
    {
        DEBUG("Unknown sensor brand: %s", sensorBrand);
        cJSON_AddItemToArray(cJSON_GetObjectItem(ret_obj, "error_messages"), cJSON_CreateString("Unknown sensor"));
        ret = -1;
    }


    return ret;
}
