/**
 *   cron_json.h
 *
 *   Provides functionality for installing a JSON-formatted schedule
 *   on the machine.
 */

#ifndef __CRON_JSON_H__

#define __CRON_JSON_H__

#include "cron.h"
#include "../cJSON/cJSON.h"

/*
 * An example of a Cron schedule is the following:
 *  {
 *     "schedule": 
 *          [
 *              {
 *          		"day":	"Saturday",
 *		            "start_time":	"0:00",
 *		            "finish_time":	"23:59",
 *		            "interval":	"2"
 *              }, 
 *              {
 *              	"day":	"Sunday",
 *                  "start_time":	"0:00",
 *                  "finish_time":	"23:59",
 *                  "interval":	"3"
 *              }
 *          ]
 *  }
 * 
 *  The day must be "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
 *  "Saturday", or "Sunday". This is case insensitive.
 *  
 *  Start time is a string "XX:YY", where XX is a number between 0 and 23,
 *  and YY is a number between 0 and 59 (inclusive). 
 *  
 *  Finish time is the same as end time. The finish time should not be before
 *  the start time.
 *  
 *  Interval is the minutes between readings. This should be a number between 
 *  1 and 59 (inclusive).
 */

/*
 *   Converts a cJSON object that represents a CronSchedule into a 
 *   list of CronSchedule objects. The executable_path gives the absolute
 *   path of the executable to be run.
 *   Returns NULL if no valid schedules were found in the cJSON struct.
 */
struct CronSchedule * convert_json_to_cronschedule(struct cJSON * json_obj,
                                                   char * executable_path);

/*
 *    Takes a cJSON object representing a CronSchedule, and uses that object
 *    to create a crontab file for the user. A copy of the user_crontab_path
 *    that was installed is saved in user_crontab_path. A copy of the json
 *    object is saved as a string in user_json_path. It is saved as an
 *    array, without enclosing curly braces. All paths are absolute.
 */ 
int init_json_cron(struct cJSON * json_schedule,
                   char * executable_path,
                   char * user_crontab_path,
                   char * user_json_path);

#endif
