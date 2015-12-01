
#ifndef SQLITE_DATABASE_H__

#define SQLITE_DATABASE_H__

int sqlite_open(char * db_path);
void sqlite_close(void);
int sqlite_run_query(char * query, int (*callback)(void*,int,char**,char**), void * param);
int sqlite_query_after(long time, int (*callback)(void*,int,char**,char**), void * param);
int sqlite_query_all(int (*callback)(void*,int,char**,char**), void * param);
int sqlite_get_read_type_id_by_string_callback(void * param, int argc, char **argv, char **azColName) ;
int sqlite_get_read_type_id_by_string(char * reading_type);

int sqlite_insert_to_readings(int readingTypeId,
                              long time,
                              char * value);
int sqlite_insert_reading(char * readingTypeAsString, long time, char * value);

int sqlite_query_readingtypes(int (*callback)(void*,int,char**,char**), void * param);

#endif

