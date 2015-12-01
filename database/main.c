#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

sqlite3 *db;
char *err_msg = 0;
int callback(void *, int, char **, char **);
int insertIntoReadings(int reading, int types, float value);

int main(int argc, char* argv[]){
    
    int rc = sqlite3_open("citizen.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    char *sql =
    "INSERT INTO ReadingTypes VALUES(NULL, 'pH', 'nil');"
    "INSERT INTO ReadingTypes VALUES(NULL, 'Conductivity', 'Siemens');";
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    }
    
    rc = insertIntoReadings(1, 0, 6.7);
   
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    }
//    sql = "SELECT * FROM ReadingTypes";
//    
//    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
//    
//    if (rc != SQLITE_OK ) {
//        
//        fprintf(stderr, "Failed to select data\n");
//        fprintf(stderr, "SQL error: %s\n", err_msg);
//        
//        sqlite3_free(err_msg);
//        sqlite3_close(db);
//        
//        return 1;
//    }
//
    sql = "SELECT * FROM Readings";
    
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    }

    
    sqlite3_close(db);
    
    return 0;

}

int insertIntoReadings(int readingtype, int types, float value){
    char *res  = malloc(500);
    char buf[32];
    
    strcpy(res, "INSERT INTO Readings VALUES(NULL, ");
    sprintf(buf, "%d", types);
    strcat(res, buf);
    strcat(res, ", ");
    sprintf(buf, "%d", readingtype);
    strcat(res, buf);
    strcat(res, ", '6/23/2015', '4:34:32 AM', ");
    sprintf(buf, "%f", value);
    strcat(res, buf);
    strcat(res, ");");
    int rc = sqlite3_exec(db, res, 0, 0, &err_msg);
    return rc;
}


int callback(void *NotUsed, int argc, char **argv,
             char **azColName) {
    
    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}