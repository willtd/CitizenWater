// log.h

#ifndef LOG_H_

#include <errno.h>
#include <string.h>

#define LOG_H_

typedef enum log_level_t {
    SHOW_ALL,
    DEBUG_LEVEL,
    INFO_LEVEL,
    NOTICE_LEVEL,
    WARN_LEVEL,
    ERROR_LEVEL,
    CRITICAL_LEVEL,
    ALERT_LEVEL,
    EMERGENCY_LEVEL,
    HIDE_ALL
} log_level_t;

/*
 *  ident is an identifier for the program 
 *  use_stderr is 1 if messages should also be printed to stderr,
 *  and 0 otherwise.
 * 
 *  This function must be called otherwise NO logging messages will be printed
 */
void enable_syslog(const char * ident, int use_stderr);

/*
 *  Sets the log level. 
 *  If level == HIDE_ALL, then no messages will be printed.
 *  If level == SHOW_ALL, all messages will be printed.
 *
 *  Otherwise, messages with a level strictly below the given
 *  parameter will be printed. For example, if WARN_LEVEL is given, messages
 *  of types DEBUG, INFO, and NOTICE will be hidden.
 *
 */
void set_log_level(enum log_level_t level);

/*
 *  NO logging messages will be printed if this has been called
 */
void disable_syslog();

void _log(enum log_level_t level, char * fmt, ...);

// Convenient macros
#define DEBUG(fmt, ...)     (_log(DEBUG_LEVEL, fmt, ##__VA_ARGS__))
#define INFO(fmt, ...)      (_log(INFO_LEVEL, fmt,  ##__VA_ARGS__))
#define WARN(fmt, ...)      (_log(WARN_LEVEL, fmt,  ##__VA_ARGS__))
#define ERROR(fmt, ...)     (_log(ERROR_LEVEL, fmt, ##__VA_ARGS__))
#define CRITICAL(fmt, ...)  (_log(CRITICAL_LEVEL, fmt, ##__VA_ARGS__))
#define ALERT(fmt, ...)     (_log(ALERT_LEVEL, fmt, ##__VA_ARGS__))
#define EMERGENCY(fmt, ...) (_log(EMERGENCY_LEVEL, fmt, ##__VA_ARGS__))

// If the failure of a function sets errno, then we can use this to print out the error
#define PERROR(msg) (_log(ERROR_LEVEL, "%s: %s\n", msg, strerror(errno)))

#endif
