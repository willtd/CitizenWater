

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "database.h"

#define BUFFER_SIZE     512		 

static sqlite3 * db;

int sqlite_open(char * db_path)
{
    return (sqlite3_open(db_path, &db) != SQLITE_OK) ? -1 : 0;
}

void sqlite_close(void) {
	sqlite3_close(db);
    db = NULL;
}

int sqlite_run_query(char * query, int (*callback)(void*,int,char**,char**), void * param)
{
    return sqlite3_exec(db, query, callback, param, NULL);
}

int sqlite_query_after(long time, int (*callback)(void*,int,char**,char**), void * param) {
    char query[BUFFER_SIZE] = {0};
    sprintf(query, "SELECT ReadingTypes.Name, Readings.Value, Readings.Time \
            FROM Readings \
            NATURAL JOIN ReadingTypes \
            WHERE Time > %ld", time);
    return (sqlite_run_query(query, callback, param) != SQLITE_OK) ? -1 : 0;
}

/*
 *  Returns -1 on error, 0 on success
 */
int sqlite_query_all(int (*callback)(void*,int,char**,char**), void * param) {
    char * query = "SELECT * FROM Readings";
    return (sqlite_run_query(query, callback, param) != SQLITE_OK) ? -1 : 0;
}

/*
 *  Returns -1 on error, 0 on success
 */
int sqlite_query_readingtypes(int (*callback)(void*,int,char**,char**), void * param) {
    char * query = "SELECT * FROM ReadingTypes";
    return (sqlite_run_query(query, callback, param) != SQLITE_OK) ? -1 : 0;
}

/*
 *  Callback for sqlite_get_read_type_by_string
 */
int sqlite_get_read_type_id_by_string_callback(void * param, int argc, char **argv, char **azColName) 
{
    int * readingTypeId = param;

    *readingTypeId = atoi(argv[0]);

    return 0;
}

/*
 *  Obtains ID for corresponding reading_type string. Assuming unique.
 *  -1 on error or the readingTypeId on success
 */ 
int sqlite_get_read_type_id_by_string(char * reading_type)
{
    char query[BUFFER_SIZE] = {0};
    int readingTypeId = -1;
    int ret = 0;

    sprintf(query, "SELECT * FROM ReadingTypes WHERE Name='%s'", reading_type);
    ret = sqlite_run_query(query, sqlite_get_read_type_id_by_string_callback, &readingTypeId);

    return (ret != SQLITE_OK) ? -1 : readingTypeId;
}

/*
 *  -1 on error, 0 on success
 */
int sqlite_insert_to_readings(int readingTypeId,
                              long time,
                              char * value)
{
    char cmd[BUFFER_SIZE] = {0};

    sprintf(cmd, 
            "INSERT INTO Readings(ReadingID,ReadingTypeID, Time, Value) VALUES(NULL, %d, %ld, '%s');",
            readingTypeId,
            time,
            value);   

    printf("Running command: %s\n", cmd);
    int ret = sqlite3_exec(db, cmd, 0, 0, NULL);

    return (ret != SQLITE_OK) ? -1 : 0;
}

/*
 *  -1 on error, 0 on success
 */
int sqlite_insert_reading(char * readingTypeAsString, long time, char * value)
{
    int readingTypeId = sqlite_get_read_type_id_by_string(readingTypeAsString);

    if (readingTypeId == -1)
    {
        printf("Reading Type ID not found\n");
        return -1;
    }
    else
    {
        return sqlite_insert_to_readings(readingTypeId,
                                         time,
                                         value);
    }
}




