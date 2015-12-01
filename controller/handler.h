/*
 *  handler.h
 */

#ifndef HANDLER_H_

#define HANDLER_H_

#include "../cJSON/cJSON.h"

/*
 *
 *  json_msg is a JSON-formatted string. This function returns a 
 *  JSON-formatted string which should be freed with free(). The contents of the
 *  message are: 
 *  {
 *      "success": [true|false]
 *  }
 *
 *  If the input message contained an "init" request, the message additionally contains:
 *      "deviceName": "myDeviceName",
 *      "sensors": [ ... ],
 *      "schedule": [ ... ]
 *
 */
char * handle_msg(char * json_msg);

#endif
