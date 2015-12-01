#include <stdio.h>
#include <stdlib.h>

#include "bluetooth-citizen.h"
#include "../cJSON/cJSON.h"


int test_sych_databases()
{
	int arraysize;
	int i;
	int j;
	cJSON* message;
	cJSON *table = cJSON_CreateArray();
	cJSON *msgTable;

	if(sych_databases(1, table)) return 1;
	arraysize = cJSON_GetArraySize(table);
	for(i = 0; i < arraysize; i += 2 )
	{
		msgTable = cJSON_CreateArray();
		message = cJSON_CreateObject();
		if(arraysize - i > 2)
		{
			for(j = 0; j < 2; j++)
			{
				cJSON_AddItemToArray(msgTable,cJSON_DetachItemFromArray(table,0));
			}
			cJSON_AddItemToObject(message, "message", cJSON_CreateString("continue"));
		}
		else
		{
			for(j = 0; j < (arraysize - i); j++)
			{
				cJSON_AddItemToArray(msgTable,cJSON_DetachItemFromArray(table,0));

			}
			cJSON_AddItemToObject(message, "message", cJSON_CreateString("end"));
		}
		cJSON_AddItemToObject(message, "table", msgTable);
		printf("%s\n",cJSON_Print(message));
		cJSON_Delete(message);
	}

	return 0;
}

int main(int argc, char * argv[]) {
	int fail = 0;
	printf("==== START TESTING bluetooth-client ==== \n");
 	printf("test sych databases\n"); fail += test_sych_databases();
	printf("failed tests %d\n", fail);
	printf("==== END TESTING bluetooth-client ==== \n");
	return 0;
}

