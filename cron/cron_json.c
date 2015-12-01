
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>

#include "cron_json.h"
#include "../shared/util.h"
#include "../shared/log.h"
#include "../shared/macros.h"

static int get_num_from_string(char * str, int start_index, int * next_index)
{
    int i = start_index;
    int num = 0;
    int scale = 1;
    while (isdigit(str[i])) //str[i] >= '0' && str[i] <= '9')
    {
        num = num * scale + (str[i] - '0');
        scale *= 10;
        i++;
    }

    if (next_index != NULL)
    {
        *next_index = i;
    }
    return num;
}

/*
 *  Strings should be of form "X:Y", where X is an integer in [0,23] and 
 *  Y is an integer in [0,59]. Returns 0 if the string was of valid form,
 *  and -1 otherwise. *hour and *min are set to 0 if the string was invalid.
 */
static int get_cron_hourmin_time(char * str, int * hour, int * min)
{
    int ind = 0;
    *hour = get_num_from_string(str, 0, &ind);

    if (*hour <= 23 && *hour >= 0)
    {
        if (str[ind] == ':')
        {
            ind++;
            *min = get_num_from_string(str, ind, &ind);
            if (*min <= 59 && *min >= 0)
            {
                if (str[ind] == '\0')
                {
                    return 0;
                }
            }
        }
    }

    *hour = 0;
    *min  = 0;
    return -1;
}

/*
 *   Returns -1 if the string is not a day of the week, and a number between
 *   0 to 6 corresponding to the appropriate day otherwise.
 */
static enum CronDayOfWeek get_day_as_int(char * day)
{
    static char * days[] = {"SUNDAY" , "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY",
                            "FRIDAY", "SATURDAY"}; // Sunday = 0, Saturday = 6
    int i = 0;
    for (i = 0; i < 7; i++)
    {
        if (!strcasecmp(days[i], day))
            return i;
    }

    return -1;
}

/*
 *  Sets up a CronSchedule object based on a cJSON object
 */
int setup_cronschedule(struct cJSON * schedule_json,
                       struct CronSchedule * schedule,
                       char * executable_path)
{
    struct cJSON * json_obj = cJSON_GetObjectItem(schedule_json, "day");

    if (json_obj != NULL && json_obj->type ==  cJSON_String)
    {
        schedule->day_of_week = get_day_as_int(json_obj->valuestring);

        if (schedule->day_of_week != -1)
        {
            json_obj = cJSON_GetObjectItem(schedule_json, "start_time");
            if (json_obj != NULL && json_obj->type ==  cJSON_String)
            {
                if (get_cron_hourmin_time(json_obj->valuestring, 
                                      &schedule->start_hour, 
                                      &schedule->start_minute) != -1)
                {                
                    json_obj = cJSON_GetObjectItem(schedule_json, "finish_time");
                    if (json_obj != NULL && json_obj->type ==  cJSON_String)
                    {
                        if (get_cron_hourmin_time(json_obj->valuestring, 
                                              &schedule->finish_hour, 
                                              &schedule->finish_minute) != -1)
                        {                
                            json_obj = cJSON_GetObjectItem(schedule_json, "interval");
                            if (json_obj != NULL && json_obj->type ==  cJSON_String)
                            {
                                if (is_string_num(json_obj->valuestring))
                                {
                                    schedule->interval_minutes = atoi(json_obj->valuestring);

                                    if (schedule->interval_minutes >= 1 && schedule->interval_minutes <= 59)
                                    {
                                        schedule->executable_path = executable_path; 
                                        return 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return -1;
}

struct CronSchedule * convert_json_to_cronschedule(struct cJSON * json_obj,
                                                   char * executable_path)
{
    if (json_obj != NULL)
    {
        struct CronSchedule * list_head = NULL;
        if (json_obj->type == cJSON_Array && !strcmp(json_obj->string, "schedule"))
        {
            DEBUG("Starting to convert schedule...\n");
            int i;
            for (i = 0; i < cJSON_GetArraySize(json_obj); i++)
            {   
                // Each item of the JSON array represents a CronSchedule  
                struct CronSchedule * current_cron = ALLOC(struct CronSchedule, 1);           
                struct cJSON * current_json = cJSON_GetArrayItem(json_obj, i);
                
                if (setup_cronschedule(current_json, current_cron, executable_path) == -1)
                {
                    DEBUG("Failed to setup CronSchedule\n");
                    free(current_cron);
                    free_cron_schedule(list_head);
                    list_head = NULL;
                    break;
                }
                else
                {
                    current_cron->next = list_head;
                    list_head = current_cron;
                }
            }
        }

        return list_head;
    }
    else
    {
        return NULL;
    }
}

int init_json_cron(struct cJSON * json_schedule,
                   char * executable_path,
                   char * user_crontab_path,
                   char * user_json_path)
{

    struct CronSchedule * cron_schedule = NULL;

    DEBUG("init_json_cron(): Converting JSON schedule to CronSchedule...\n");
    cron_schedule = convert_json_to_cronschedule(json_schedule, executable_path);

    if (cron_schedule == NULL)
    {
        ERROR("init_json_cron(): Failed to convert JSON schedule\n");
        return -1; 
    }
    else
    {
        DEBUG("init_json_cron(): Attempting to install cron schedule...\n");
        
        if (init_cron(cron_schedule, user_crontab_path) == -1)
        {
            ERROR("init_cron failed\n. Failed to install schedule.");
            free_cron_schedule(cron_schedule);
            return -1;
        }
        else
        {
            if (user_json_path != NULL)
            {
                FILE * file = fopen(user_json_path, "w");
                if (file != NULL)
                {
                    fprintf(file, "%s", cJSON_Print(json_schedule));
                    fclose(file);
                }

            }
        }

        free_cron_schedule(cron_schedule);
    }


    return 0; 
}

