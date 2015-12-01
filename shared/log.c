/*
 * log.c
 *
 * Implementation of simple logging library
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <syslog.h>

#include "log.h"

static int    log_opened = 0;

static char * get_log_level_as_string(int level) 
{
    switch (level) 
    {
	    case SHOW_ALL:        return "SHOW ALL";
        case DEBUG_LEVEL:     return "DEBUG";
        case INFO_LEVEL:      return "INFO";
        case WARN_LEVEL:      return "WARN";
	    case ERROR_LEVEL:     return "ERROR";
        case CRITICAL_LEVEL:  return "CRITICAL";
        case ALERT_LEVEL:     return "ALERT";
        case EMERGENCY_LEVEL: return "EMERGENCY";
	    case HIDE_ALL:        return "HIDE ALL";
        default:              return "UNKNOWN";
    }

    return "UNKNOWN";
}

static int get_syslog_level(int level) 
{
    switch (level) 
    {
        case DEBUG_LEVEL:     return 7;
        case INFO_LEVEL:      return 6;
        case WARN_LEVEL:      return 5;
	    case ERROR_LEVEL:     return 4;
        case CRITICAL_LEVEL:  return 3;
        case ALERT_LEVEL:     return 2;
        case EMERGENCY_LEVEL: return 1;
        default:              return 0;
    }

    return 7;
}

void enable_syslog(const char * ident, int use_stderr)
{
    if (log_opened)
    {
        closelog();
    }
   
    openlog(ident, (use_stderr == 1) ?  LOG_PERROR : 0, 0);
    log_opened = 1;
}

void disable_syslog()
{
    log_opened = 0;
    closelog();
}

void set_log_level(enum log_level_t level) 
{
    int mask = 0;
    switch (level)
    {
        case SHOW_ALL:        mask = 0xFF; break;
        case DEBUG_LEVEL:     mask = 0xFF; break;
        case INFO_LEVEL:      mask = 0x7F; break;
        case NOTICE_LEVEL:    mask = 0x3F; break;
        case WARN_LEVEL:      mask = 0x1F; break;
        case ERROR_LEVEL:     mask = 0x0F; break; 
        case CRITICAL_LEVEL:  mask = 0x07; break;
        case ALERT_LEVEL:     mask = 0x03; break;
        case EMERGENCY_LEVEL: mask = 0x01; break;
        case HIDE_ALL:        mask = 0x00; break;
    }
    
    setlogmask(mask);
}

void _log(enum log_level_t msg_level, char * fmt, ... )
{
    char msg_buffer[4096] = {0}; // Limiting maximum message size to 4K
    
    if (log_opened)
    {
        va_list list;
        va_start(list, fmt);
        vsnprintf(msg_buffer, sizeof(msg_buffer), fmt, list);
        syslog(get_syslog_level(msg_level), "[%s] %s", get_log_level_as_string(msg_level), msg_buffer);
        va_end(list);
    }
}
