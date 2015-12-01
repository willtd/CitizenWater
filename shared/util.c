// util.c

#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "log.h"
#include "util.h"
#include "macros.h"

int path_exists(char * path)
{
    if (!access(path, F_OK))
	    return 1;
    else 
	    return 0;
}

int remove_file(char * path)
{
    if (path_exists(path))
    {
	    int ret = unlink(path);
	    if (ret == -1)
	    {
	        PERROR("unlink");
	        return -1;
	    }
	    return 0;
    }
    return 0;
}

int is_string_num(char * str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

void get_current_time(char * buff, int size)
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buff, size, "%D %T", timeinfo);
}

char * get_file_as_string(char * file_path)
{
    char * buffer = 0;
    long length;
    FILE * f = fopen (file_path, "rb");

    if (f) // http://stackoverflow.com/questions/174531/easiest-way-to-get-files-contents-in-c
    {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = malloc (length);
      if (buffer)
      {
        fread (buffer, 1, length, f);
      }
      fclose (f);
    }

    return buffer;
}

long get_unix_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long) tv.tv_sec;
}
