/*
 *  daemon_options.c
 *
 *  Handles parsing and printing of command line options of the sensor-daemon.
 *  See usage() for available command line options.
 */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "../shared/macros.h" // PATH_SIZE
#include "../shared/log.h"    // SHOW_ALL, HIDE_ALL macros

#include "daemon_options.h"

void usage() 
{
    printf("Usage: \n");
    printf("sensor-daemon [OPTION...] \n\n");
    printf("Options:\n");
    printf("\t--help              \tShow help\n");
    printf("\t--log-level=LEVEL   \tChange log level\n");
}

void print_options(daemon_options_t * opts)
{
    fprintf(stderr, "Log level: %d\n", opts->log_level);
}

void parse_options(int argc, char * argv[], daemon_options_t * opts)
{
    int ret = 0;
    const char* optstring = "";
    const struct option longopts[] = {
        { "help", 0, NULL, 'h' },
        { "log-level", 1, NULL, 'l' },
        { NULL,        0, NULL,  0  } };

    // Assign default values
    opts->log_level = SHOW_ALL;

    do {
        ret = getopt_long(argc, argv, optstring, longopts, NULL);
        switch (ret)    
        {
            case 'h': 
                usage();
                exit(0);
                break;
            case 'l': 
                opts->log_level = atoi(optarg);
                if (opts->log_level < SHOW_ALL || opts->log_level > HIDE_ALL)
                {
                    printf("Log level must be between %d and %d\n", SHOW_ALL, HIDE_ALL);
                    exit(1);
                }
                break;
            case '?': 
                usage();
                exit(1);
                break;
            case -1:  
                break;
            default:
                printf("Error occurred in parsing options\n");
                exit(1);
                break;
        }
    } while (ret != -1);
}

