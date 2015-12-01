/*
 *  handler.c
 * 
 *  Contains the logic of actually handling received JSON messages
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../cron/cron_json.h"
#include "../cJSON/cJSON.h"
#include "../shared/log.h"
#include "../shared/util.h"
#include "../shared/macros.h"

#include "handler.h"
#include "sensor_handlers.h"

/*
 *   Hardcoded paths
 */
#define USER_CRONTAB_FILE        "/home/citizenwater/my_crontab.cron"
#define BACKUP_CRONTAB_JSON_FILE "/home/citizenwater/cron.json"
#define USER_EXECUTABLE          "/opt/citwater/cron-script.sh"
#define SENSOR_INFO_FILE         "/home/citizenwater/sensors.json"


void add_json_msg(struct cJSON * ret_obj, char * msg)
{
    struct cJSON * msg_array = cJSON_GetObjectItem(ret_obj, "messages");

    if (msg_array != NULL)
    {
        cJSON_AddItemToArray(msg_array, cJSON_CreateString(msg));
    }

}


void add_json_error_msg(struct cJSON * ret_obj, char * msg)
{
    struct cJSON * msg_array = cJSON_GetObjectItem(ret_obj, "error_messages");

    if (msg_array != NULL)
    {
        cJSON_AddItemToArray(msg_array, cJSON_CreateString(msg));
    }
}

int get_device_name(char* buff, int len)
{
    int ret = gethostname(buff, len - 1);
    if (ret == 0)
    {
        buff[len-1] = '\0';
    }
    return ret;
}

char * get_string_from_cJSON_object(cJSON * obj, char * name)
{
    cJSON * named_obj = cJSON_GetObjectItem(obj, name);
    if (named_obj != NULL && named_obj->type == cJSON_String)
    {
        return named_obj->valuestring;
    }
    else
    {
        return NULL;
    }
}

/*
 *   Returns a JSON object containing the scheduling that the microcontroller
 *   is currently using. 
 */
struct cJSON * get_cron_schedule_as_json()
{
    char * schedule_string = get_file_as_string(BACKUP_CRONTAB_JSON_FILE);

    if (schedule_string == NULL)
    {
        ERROR("get_cron_schedule_as_json(): failed to get contents of path %s\n",
                                            BACKUP_CRONTAB_JSON_FILE);
        return NULL;
    }

    cJSON * schedule_obj = cJSON_Parse(schedule_string);

    free(schedule_string);

    return schedule_obj;
}

struct cJSON * get_sensor_info_as_json()
{
    char * sensor_string = get_file_as_string("/home/citizenwater/sensors.json");

    if (sensor_string == NULL)
    {
        ERROR("get_sensor_info_as_json(): failed to get contents of path %s\n", SENSOR_INFO_FILE);
        return NULL;
    }

    cJSON * sensor_obj = cJSON_Parse(sensor_string);
    free(sensor_string);
    return sensor_obj;
}

/*
 *   Updates the scheduling to that referred to by the cJSON object.
 *   Returns 0 on success and -1 on failure.
 */
int handle_set_cron_schedule(struct cJSON * schedule_obj, struct cJSON * ret_obj)
{
    // Could backup old schedule here; for now, just overwriting the old schedule
    int ret = 0;

    remove_file(USER_CRONTAB_FILE);
    ret =  init_json_cron(schedule_obj, // Install new schedule
                          USER_EXECUTABLE,
                          USER_CRONTAB_FILE,
                          BACKUP_CRONTAB_JSON_FILE);

    if (ret == -1)
    {
        add_json_error_msg(ret_obj, "Error occurred in installing schedule on device");
    }

    return ret;
}

/*
 *   Sets up the message the device responds with after communication is
 *   established. Contains basic information about the device, including:
 *   
 *   "deviceName"
 *   "sensors"
 *   "schedule"
 */
int handle_init_msg(struct cJSON * ret_obj)
{
    int ret = 0;

    // Add device_name to return object
    #define MAX_DEV_NAME_LEN 50 // includes null-terminating char
    char device_name[MAX_DEV_NAME_LEN];
    ret = get_device_name(device_name, MAX_DEV_NAME_LEN);

    if (ret == 0)
    {
        cJSON_AddStringToObject(ret_obj, "deviceName", device_name);
    }
    else
    {
        ERROR("handle_init_msg(): Failed to retrieve device name\n");
        add_json_error_msg(ret_obj, "Error occurred in retrieving name of device");
        ret = -1;
    }


    cJSON * sensor_obj = get_sensor_info_as_json();
    if (sensor_obj != NULL)
    {
	    cJSON_AddItemToObject(ret_obj, "sensors", cJSON_GetObjectItem(sensor_obj, "sensors"));
    }
    else
    {
        ERROR("handle_init_msg(): Failed to retrieve sensor info\n");
        add_json_error_msg(ret_obj, "Error occurred in retrieving sensor information on device");
        ret = -1;
    }

    cJSON * schedule_obj = get_cron_schedule_as_json();
    if (schedule_obj != NULL)
    {
	    cJSON_AddItemToObject(ret_obj, "schedule", schedule_obj);
    }
    else
    {
        ERROR("handle_init_msg(): Failed to retrieve scheduling info\n");

        add_json_error_msg(ret_obj, "Error occurred in retrieving scheduling information on device");
        ret = -1;
    }

    return ret;
}

/*
 *  Process actions on the sensors. Will involve the i2c bus, so let's acquire
 *  the lock. Possible actions here including taking a reading or calibrating
 *  the sensors. 
 */
int handle_sensor_msg(struct cJSON * obj, struct cJSON * ret_obj) 
{
    int i = 0, ret = 0;

    if (obj->type == cJSON_Array)
    {
        for (i = 0; i < cJSON_GetArraySize(obj); i++) 
        {
            cJSON * current = cJSON_GetArrayItem(obj,i);

            char * sensorType  = get_string_from_cJSON_object(current, "sensorType");
            char * sensorBrand = get_string_from_cJSON_object(current, "sensorBrand");

            cJSON * actionsObj = cJSON_GetObjectItem(current, "actions");

            if (cJSON_GetObjectItem(actionsObj, "calibrate") != NULL)
            {
                INFO("handle_sensor_msg(): Calibrating sensor %s\n", sensorType);
                int ret2 = handle_calibrate(sensorType, sensorBrand, cJSON_GetObjectItem(actionsObj, "calibrate"), ret_obj);
                ret = (ret == 0) ? ret2 : -1;
            }
            if (cJSON_GetObjectItem(actionsObj, "take_reading") != NULL)
            {
                INFO("handle_sensor_msg(): Reading from sensor %s\n", sensorType);
                int ret2 = handle_take_reading(sensorType, sensorBrand, cJSON_GetObjectItem(actionsObj, "take_reading"), ret_obj);
                ret = (ret == 0) ? ret2 : -1;
            }
        }

        return ret;
    }
    else
    {
        ERROR("handle_sensor_msg(): JSON message was formatted improperly -- \"sensors\" must be a JSON array\n");
        add_json_error_msg(ret_obj, "Invalid message received -- improperly formatted.");

        return -1;
    }
}

char * handle_msg(char * json_msg) 
{
    int ret = 0;
    struct cJSON * json_msg_obj = cJSON_Parse(json_msg);   
    struct cJSON * return_obj = cJSON_CreateObject();
    struct cJSON * return_msg_array = cJSON_CreateArray();
    struct cJSON * return_error_msg_array = cJSON_CreateArray();

    INFO("handle_msg(): Handling new message...\n");
    
    if ((return_obj == NULL) || (return_msg_array == NULL) || (return_error_msg_array == NULL))
    {
        EMERGENCY("handle_msg(): Failed to allocate room on heap");
        ret = -1;
    }

    cJSON_AddItemToObject(return_obj, "messages", return_msg_array);
    cJSON_AddItemToObject(return_obj, "error_messages", return_error_msg_array);

    if (json_msg_obj == NULL)
    {
        ERROR("handle_msg(): Failed to parse JSON message. " 
              "Perhaps the message was not a valid JSON message: \n%s\n", json_msg);

        add_json_error_msg(return_obj, "Invalid message received -- Unable to parse contents.");

        ret = -1;
    }

    if (ret == 0)
    {

        if (cJSON_GetObjectItem(json_msg_obj, "init") != NULL)
        {
            INFO("handle_msg(): Handling 'init' message...\n");

            ret = handle_init_msg(return_obj);
        }
        else
        {
            struct cJSON * sched = cJSON_GetObjectItem(json_msg_obj , "schedule");
            if (sched != NULL)
            {
                INFO("handle_msg(): Handling 'schedule' message...\n");
                ret = handle_set_cron_schedule(sched, return_obj);
            }

            struct cJSON * sensors = cJSON_GetObjectItem(json_msg_obj, "sensors");
            if (sensors != NULL)
            {
                INFO("handle_msg(): Handling 'sensors' message...\n");
                ret = (handle_sensor_msg(sensors, return_obj) == -1) ? -1 : ret;
            }
        }

        cJSON_Delete(json_msg_obj);
    }
    else
    {
        ret = -1;
    }

    cJSON_AddBoolToObject(return_obj, "success", (ret == 0) ? cJSON_True : cJSON_False);

    char * return_str = cJSON_Print(return_obj);

    cJSON_Delete(return_obj);

    return return_str;
}

