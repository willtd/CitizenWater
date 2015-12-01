#include <unistd.h>

#include "cron.h"
#include "../shared/macros.h"
#include "../shared/log.h"
#include "../shared/util.h"

#define CRON_EXECUTABLE   "/usr/bin/crontab"
#define MIN_HOUR   0
#define MIN_MINUTE 0
#define MAX_HOUR   23
#define MAX_MINUTE 59

static int error_check_cron_schedule(FILE * fptr,
                         struct CronSchedule * schedule);

static int print_cron_trigger(FILE * fptr,
                        int start_hour,      // 0 to 23
                        int start_minute,    // 0 to 59
                        int finish_hour,     // 0 to 23
                        int finish_minute,   // 0 to 59
                        int interval_minutes,// 0 to 59
                        int day_of_week,
                        char * executable_path);     // 1 to 7 [SUN-SAT]


static int print_cron_schedule(FILE * fptr,
                         struct CronSchedule * schedule);

/*
 *  Dynamically allocates a CronSchedule struct with the given parameters
 */
struct CronSchedule * create_cron_node(
                            int start_hour,      // 0 to 23
                            int start_minute,    // 0 to 59
                            int finish_hour,     // 0 to 23
                            int finish_minute,   // 0 to 59
                            int interval_minutes,// 0 to 59
                            int day_of_week,
                            char * executable_path)     // 1 to 7 [SUN-SAT]
{
    struct CronSchedule * node = ALLOC(struct CronSchedule, 1);

    if (node == NULL)
    {
        DEBUG("Memory allocation failed.\n");
    }
    node->start_hour       = start_hour;
    node->start_minute     = start_minute;
    node->finish_hour      = finish_hour;
    node->finish_minute    = finish_minute;
    node->interval_minutes = interval_minutes;
    node->day_of_week      = day_of_week;
    node->executable_path  = executable_path;
    node->next             = NULL;
    
    return node;
}

/*
 *  Note: Inputs are not checked for errors
 */
static int print_cron_trigger(FILE * fptr,
                        int start_hour,      // 0 to 23
                        int start_minute,    // 0 to 59
                        int finish_hour,     // 0 to 23
                        int finish_minute,   // 0 to 59
                        int interval_minutes,// 0 to 59
                        int day_of_week,     // 1 to 7 [SUN-SAT] 
                        char * executable_path)
{
    if (start_hour == finish_hour)
    {
        fprintf(fptr,
                "%d-%d/%d %d * * %d %s\n",
                start_minute,
                finish_minute,
                interval_minutes,
                start_hour,
                day_of_week,
                executable_path);
    }
    else
    {
        fprintf(fptr,
                "%d-%d/%d %d-%d * * %d %s\n",
                start_minute,
                finish_minute,
                interval_minutes,
                start_hour,
                finish_hour,
                day_of_week,
                executable_path);
    }

    return 0;
}

/*
 *  Checks a CronSchedule for invalid numbers; returns 0 if schedule is valid
 *  and -1 if it is invalid
 */
static int error_check_cron_schedule(FILE * fptr,
                         struct CronSchedule * schedule)
{
    if (schedule != NULL)
    {
        int start_hour = schedule->start_hour;
        int finish_hour = schedule->finish_hour;
        int start_minute = schedule->start_minute;
        int finish_minute = schedule->finish_minute;
        int day_of_week = schedule->day_of_week;
        int interval_minutes = schedule->interval_minutes;

        if (start_hour > finish_hour)
        {
        }
        else if (start_hour == finish_hour && start_minute > finish_minute)
        {
        }
        else if (day_of_week < CRON_SUN || day_of_week > CRON_SAT)
        {
        }
        else if (start_hour < MIN_HOUR || start_hour > MAX_HOUR)
        {
        }
        else if (start_minute < MIN_MINUTE || start_minute > MAX_MINUTE)
        {
        }
        else if (interval_minutes < MIN_MINUTE || interval_minutes > MAX_MINUTE)
        {
        }
        else
        {
            return 0;
        }
    }
    return -1;
}

/* 
 *   Prints the CronSchedule to the given file. Returns -1 if an error occurred,
 *   and 0 if the schedule was successfully printed.
 */
static int print_cron_schedule(FILE * fptr,
                               struct CronSchedule * schedule)
{
    int start_hour = schedule->start_hour;
    int finish_hour = schedule->finish_hour;
    int start_minute = schedule->start_minute;
    int finish_minute = schedule->finish_minute;

    if (error_check_cron_schedule(fptr, schedule) == -1)
    {
        DEBUG("Bad schedule\n");
        return -1;
    }
    else if (start_hour == finish_hour && start_minute <= finish_minute)
    {   // Print start_hour:start_minute to finish_hour:finish_minute
        print_cron_trigger(fptr, 
                           schedule->start_hour,
                           schedule->start_minute,
                           schedule->finish_hour,
                           schedule->finish_minute,
                           schedule->interval_minutes,
                           schedule->day_of_week,
                           schedule->executable_path);                        
    }
    else 
    {   // Print start_hour:start_minute to start_hour:59
        print_cron_trigger(fptr, 
                           schedule->start_hour,
                           schedule->start_minute,
                           schedule->start_hour,
                           MAX_MINUTE,
                           schedule->interval_minutes,
                           schedule->day_of_week,
                           schedule->executable_path); 

        // "middle" hours
        if (start_hour < finish_hour - 1)
        {
            print_cron_trigger(fptr, 
                               start_hour + 1,
                               0,
                               finish_hour - 1,
                               59,
                               schedule->interval_minutes,
                               schedule->day_of_week,
                               schedule->executable_path); 
        }

        // TODO: What about the case where 3:00 to 4:00?
        // Currently prints two lines, is this ok?

        // Print finish_hour:0 to finish_hour:finish_minutes
        print_cron_trigger(fptr, 
                           schedule->finish_hour,
                           0,
                           schedule->finish_hour,
                           schedule->finish_minute,
                           schedule->interval_minutes,
                           schedule->day_of_week,
                           schedule->executable_path); 
    }

    return 0;
}

int print_cron_schedule_list(FILE * fptr,
                             struct CronSchedule * schedule)
{   
    int ret;
    struct CronSchedule* current = schedule;
    while (current != NULL)
    {
        ret = (print_cron_schedule(fptr, current) == -1) ? -1 : ret;
        current = current->next;
    }
    return ret;
}

int install_crontab(char * file_path)
{
    char cmd[STRING_SIZE] = {0};
    sprintf(cmd, "%s %s", CRON_EXECUTABLE, file_path);

    int ret = fork();
    if (ret == 0) { 
        ret = system(cmd);

        if (ret == -1)
        {
            DEBUG("Error occurred in installing Cron schedule\n");
        }

        exit(0);
    }
    else if (ret == -1)
    {
        DEBUG("Error occurred in forking process\n");
        return -1;
    }

    sleep(5); // Ugly, but gives crontab time to install schedule

    return 0;
}

int remove_crontab()
{
    char cmd[STRING_SIZE] = {0};
    sprintf(cmd, "%s -r", CRON_EXECUTABLE);

    int ret = fork();
    if (ret == 0) { 
        ret = system(cmd);

        if (ret == -1)
        {
            DEBUG("Error occurred in installing Cron schedule\n");
        }

        exit(0);
    }
    else if (ret == -1)
    {
        DEBUG("Error occurred in forking process\n");
    }
       
    sleep(5); // Ugly, but gives crontab time to install schedule

    return ret;
}

int init_cron(struct CronSchedule * schedule,
              char * user_crontab_file)       // A file to save a copy of the user's crontab
{

    FILE * file = fopen(user_crontab_file, "w");

    if (file != NULL)
    {
        if (print_cron_schedule_list(file, schedule) == -1) 
        {
            DEBUG("An error occurred in creating the Cron schedule.\n");
            fclose(file);
            return -1;
        }
        else
        {
            fclose(file);
            if (install_crontab(user_crontab_file) == -1) // note: schedule in user_crontab_file gets duplicated
            {                                             // but crontab is OK
                DEBUG("An error occurred in creating the Cron schedule.\n");
                return -1;
            }
        }
        return 0;
    }

    DEBUG("An error occurred in opening the file\n");
    return -1; 
}

void free_cron_schedule(struct CronSchedule * schedule)
{
    struct CronSchedule * current = schedule;
    struct CronSchedule * back    = schedule;

    while (current != NULL)
    {
        back    = current;
        current = current->next;
        free(back);
    }
}

