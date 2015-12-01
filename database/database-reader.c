#include "database.h"
#include "math.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int sqlite_query_all_callback(void *NotUsed, int argc, char **argv,
	     char **azColName) {
    
    int i;
    for (i = 0; i < argc; i++) {
	    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}

int main(int argc, char * argv[]) {
    if (argc < 2)
    {
        printf("First command-line argument must be path of database to read\n");
    }

    if (sqlite_open(argv[1]) < 0)
    {
        printf("Open database fail\n");
    }

    printf("==== START OF READINGTYPES TABLE ==== \n");

    if (sqlite_query_readingtypes(sqlite_query_all_callback, NULL) < 0)
    {
        printf("Query all reading types fail\n");
    }

    printf("==== END OF READINGTYPES TABLE ==== \n");

    printf("==== START OF READINGS TABLE ==== \n");

    if (sqlite_query_all(sqlite_query_all_callback, NULL) < 0)
    {
        printf("Query all readings fail\n");
    }    

    printf("==== END OF READINGS TABLE ==== \n");

    sqlite_close();
        
    return 0;
}

