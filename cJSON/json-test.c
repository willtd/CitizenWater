
#include <stdlib.h>
#include <stdio.h>
#include "cJSON.h"


int main(void) {
    char str[5000] = {0};
    
    int i = 0;
    while (!feof(stdin)) {
	str[i++] = getchar();
//	str[i++] = getchar();
    }
    str[i-1] = '\0';
  //  scanf("%s", str);

    printf("%s\n", str);
    
    struct cJSON *c;
    c= cJSON_Parse(str);
    
    printf("%s\n", cJSON_Print(c));
}
