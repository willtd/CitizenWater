/*
 *  daemon_options.h
 */

#ifndef DAEMON_OPTIONS_H_

#define DAEMON_OPTIONS_H_

#include "../shared/macros.h"

#define DEFAULT_LOG "/home/citizenwater/sensor_daemon.log"

typedef struct daemon_options_t {
    int    log_level;              // Integer between SHOW_ALL and HIDE_ALL
} daemon_options_t; 

/*
 *  Prints options in the given struct to stderr
 */
void print_options(daemon_options_t * opts);

/*
 *  Parses options and places relevant values in the struct pointed to by opts
 */
void parse_options(int argc, char * argv[], daemon_options_t * opts);


#endif

