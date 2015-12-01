// sensor-daemon-main.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../shared/log.h"
#include "../shared/util.h"
#include "../atlsci-sensors/sensors.h"
#include "daemon_options.h"
#include "../database/database.h"

#define READINGS_DB  "/home/citizenwater/citizenwater.db"
#define TIME_SIZE 40

struct daemon_options_t opts;

/**
 *  Takes a reading and logs to the database
 */
void take_reading(int sensor_type)
{
    long current_time = get_unix_timestamp();
    atlsci_read_ret_t reading = atlsci_read(sensor_type);

    if (reading.success == 1)
    {
        DEBUG("%ld: %s: %s\n", current_time, get_sensor_type_as_string(sensor_type), reading.data);
        sqlite_insert_reading(get_sensor_type_as_string(sensor_type), current_time, reading.data);
    }
    else
    {
        DEBUG("%ld: %s: Error\n", current_time, get_sensor_type_as_string(sensor_type));
    }
    atlsci_sleep_ret_t sleep = atlsci_sleep(sensor_type);
}

void mock_reading(FILE * db)
{
    char buff[TIME_SIZE];

    get_current_time(buff, TIME_SIZE);

    fprintf(db, "Hello! The time is: %s\n", buff);
}

int main(int argc, char * argv[]) 
{
    parse_options(argc, argv, &opts);
    print_options(&opts);

    set_log_level(opts.log_level);
    enable_syslog("sensor-daemon", opts.log_level);

    FILE * file = fopen("/home/citizenwater/thisisatest", "a+");
    mock_reading(file);
    fclose(file);



    if (sqlite_open(READINGS_DB) < 0)
    {
        ERROR("Failed to open database\n");
    }
    else
    {

        if (atlsci_init() < 0)
        {
            ERROR("atlsci_init()");
        }
        else
        {
            take_reading(ATLSCI_DO);
            take_reading(ATLSCI_ORP);
            take_reading(ATLSCI_PH);
            take_reading(ATLSCI_CONDUCTIVITY);

            atlsci_deinit();
        }
        sqlite_close();
    }

    disable_syslog();
    return 0;
}
