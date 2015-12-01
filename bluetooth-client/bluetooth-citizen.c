#include <stdio.h>
#include <stdlib.h>

#include "bluetooth-citizen.h"
#include "../database/database.h"
int sych_databases_callback(void *param, int argc, char **argv, char **azColName)
{
	int i;
	cJSON *row = cJSON_CreateObject();
	
	for(i = 0; i < argc; i++)
	{
		cJSON_AddItemToObject(row, azColName[i], cJSON_CreateString(argv[i]));
	}
	cJSON_AddItemToArray((cJSON*)param, row);
	return 0;
}

int sych_databases(long time, cJSON *object)
{
	if(sqlite_open("/home/citizenwater/citizenwater.db"))
	{
		printf("datase failed to open\n");
		return -1;
	}
	
       	if(sqlite_query_after(time, sych_databases_callback, object))
	{
		printf("queury after failed\n");
		return -1;
	}
	
	sqlite_close();
	return 0;
}
