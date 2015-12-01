/*
 *  util.h
 */
#ifndef UTIL_H_

#define UTIL_H_

/*
 * Unlinks the file at the given path. If the path does not exist,
 * does nothing.
 * Returns 0 on success and -1 on failure.
 */
int remove_file(char * path);

int is_string_num(char * str);

/* 
 *  Writes a string of the following form to buff: 06/08/15 14:31:27
 *  buff should be of sufficient size, otherwise clipping will occur.
 */ 
void get_current_time(char * buff, int size);

/*
 *  Returned pointer can be freed.
 */
char * get_file_as_string(char * file_path);

long get_unix_timestamp(void);

#endif
