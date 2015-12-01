// macros.h

#ifndef MACROS_H_

#define MACROS_H_

#include <stdlib.h>

#define STRING_SIZE 4096
#define PATH_SIZE   4096 // TODO: Check this number
#define JSON_SIZE   16384 

#define ALLOC(type, n) ((type*) calloc(n, sizeof(type)))

#endif
