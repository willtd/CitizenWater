
#include "database.h"
#include "math.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define DB_NAME		"/home/citizenwater/test.db"

long get_unix_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long) tv.tv_sec;
}

static int sqlite_query_all_callback(void *NotUsed, int argc, char **argv,
	     char **azColName) {
    
    NotUsed = 0;
    int i;
    for (i = 0; i < argc; i++) {
	printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}

int main() {
    /*
        INSERT INTO ReadingTypes VALUES(1, "Conductivity", "Unit?");
        INSERT INTO ReadingTypes VALUES(2, "Dissolved Oxygen", "Unit?");
        INSERT INTO ReadingTypes VALUES(3, "Oxygen Reduction Potential", "Unit?");
        INSERT INTO ReadingTypes VALUES(4, "pH", "Unit?");
    */
    if (sqlite_open(DB_NAME) < 0)
    {
        printf("Open database fail\n");
    }

    if (sqlite_query_readingtypes(sqlite_query_all_callback, NULL) < 0)
    {
        printf("Query all reading types fail\n");
    }

    if (sqlite_query_all(sqlite_query_all_callback, NULL) < 0)
    {
        printf("Query all readings fail\n");
    }
    if (sqlite_insert_reading("pH", get_unix_timestamp(), "6.7") < 0)
    {
        printf("Insert fail\n");
    }
    if (sqlite_insert_reading("Dissolved Oxygen", get_unix_timestamp(), "120.6") < 0)
    {
        printf("Insert fail\n");
    }
    if (sqlite_insert_reading("Oxygen Reduction Potential", get_unix_timestamp(), "10020.6") < 0)
    {
        printf("Insert fail\n");
    }
    if (sqlite_insert_reading("Conductivity", get_unix_timestamp(), "1000.6") < 0)
    {
        printf("Insert fail\n");
    }
    if (sqlite_query_all(sqlite_query_all_callback, NULL) < 0)
    {
        printf("Query all readings fail\n");
    }

    sqlite_close();
        
    return 0;
}

