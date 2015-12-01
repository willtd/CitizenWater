#ifndef _BLUETOOTH_CITIZEN_H
#define _BLUETOOTH_CITIZEN_H


struct bluetooth_msg {
    char type;                // C (for controller) or D (for database)
    int    msg_contents_len; // 0 to 9999, length does not include NT char
    char * msg_contents;     // Contents
    int valid;               // error (-1) or no (0)
};


#include "../cJSON/cJSON.h"
#include <stdlib.h>


//int process_fmtd_hdr(char * hdr, struct bluetooth_msg * msg);
//struct bluetooth_msg recv_fmtd_msg(int bluetooth_fd);
//int send_fmtd_msg(int bluetooth_fd, char type, char * contents);
//char * send_msg_to_controller(int controller_fd, char * contents);
//int setup_bluetooth_socket(void);
int sych_databases(long time, cJSON *object);
//int handle_bluetooth_client(int bluetooth_fd, int controller_fd);

#endif // _BLUETOOTH_CITIZEN_H
