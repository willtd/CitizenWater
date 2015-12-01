/*
 *  options.h
 */

#ifndef OPTIONS_H_


#define OPTIONS_H_

typedef struct controller_options_t {
    int    log_level;              // Integer between SHOW_ALL and HIDE_ALL
    char   socket_file[PATH_SIZE]; // Default path if not specified
} controller_options_t; 

/*
 *  Prints options in the given struct to stderr
 */
void print_options(controller_options_t * opts);

/*
 *  Parses options and places relevant values in the struct pointed to by opts
 */
void parse_options(int argc, char * argv[], controller_options_t * opts);


#endif

