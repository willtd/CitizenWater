
#ifndef __CRON_H__

#define __CRON_H__

#include <stdio.h>

/*
 *  A CronSchedule struct can be translated into a one-line string as follows:
 *    start_minute-finish_minute/interval_minutes
 *    start_hour-finish_hour
 *    *
 *    *
 *    day_of_week
 *    program_to_be_run
 */
struct CronSchedule {
    int start_hour;      // 0 to 23
    int start_minute;    // 0 to 59
    int finish_hour;     // 0 to 23
    int finish_minute;   // 0 to 59
    int interval_minutes;// 0 to 59
    int day_of_week;     // 1 to 7 [SUN-SAT]
    char * executable_path;
    struct CronSchedule * next;
};

enum CronDayOfWeek {
    CRON_SUN,
    CRON_MON,
    CRON_TUE,
    CRON_WED,
    CRON_THUR,
    CRON_FRI,
    CRON_SAT
};

/*
 *   Given a CronSchedule, installs the schedule for the user (any previous
 *   schedule is overwritten) and saves a backup copy of the crontab file to
 *   the path user_crontab_file (required, cannot be NULL)
 */
int init_cron(struct CronSchedule * schedule,
              char * user_crontab_file);       // A file to save a copy of the user's crontab

/*
 *   Removes the user's crontab with the command "crontab -r".
 */
int remove_crontab();

/*
 *  Installs the given file with the command: crontab file_path
 */
int install_crontab(char * file_path);

/*
 *   Takes as input a FILE pointer and a simplified Cron schedule.
 *   Prints the Cron schedule to the given file. Returns -1 on error
 *   and zero on success. If an error occurred, this indicates that the 
 *   entire schedule may not have been printed to the file.
 */
int print_cron_schedule_list(FILE * fptr,
                             struct CronSchedule * schedule);

/*
 *  Frees the CronSchedule objects in the list
 */
void free_cron_schedule(struct CronSchedule * schedule);

#endif
