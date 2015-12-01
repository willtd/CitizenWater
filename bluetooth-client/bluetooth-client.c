/*
 *  bluetooth-client.c
 *  
 *  Handles the communication between the external device and microcontroller 
 *  via Bluetooth. Passes messages between outside world and controller process.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "../controller/communication_controller.h"

#include "../shared/sockets.h"
#include "../shared/log.h"
#include "../shared/macros.h"
#include "../database/database.h"
#include "bluetooth-citizen.h"

#define BLUETOOTH_CHANNEL 22
#define MSG_SIZE 100
#define CONTROLLER_SOCKET_PATH "/tmp/water-controller-socket"

int process_fmtd_hdr(char * hdr, struct bluetooth_msg * msg)
{   
    // MESSAGE FORMAT:
    //  [1-character CHAR] [4-digit NUMBER] [CONTENTS ...]

    int i = 0;

    if (hdr[0] != 'C' && hdr[0] != 'D')
    {
        msg->valid = -1;
        return -1;
    }

    msg->type = hdr[0];

    printf("process_fmtd_hdr: type %c\n", hdr[0]);

    for (i = 1; i <= 4; i++) /* Process 4-digit portion of header*/
    {
        if (!isdigit(hdr[i]) && !(hdr[i] ==' '))
        {
            msg->valid = -1;
            return -1;
        }
        else if (isdigit(hdr[i]))
        {
	    printf("%c %d %d\n", hdr[i], hdr[i], '0');
            msg->msg_contents_len = (10*msg->msg_contents_len) + (hdr[i] - '0');
        }
    }

    printf("process_fmtd_hdr: len %d\n", msg->msg_contents_len);;

    return 0;
}

struct bluetooth_msg recv_fmtd_msg(int bluetooth_fd)
{
    struct bluetooth_msg msg = {0};
    int ret = 0;
    char hdr[6] = {0}; // 1 char + 4 digit + 1 NT char
    char * msg_contents = NULL;

    printf("Waiting in recv_fmtd_msg\n");
    ret = recv_msg_socket(bluetooth_fd, hdr, 5 /* recv_msg_socket does not include \0 */);

    printf("recv_msg_socket() returned with return value %d\n", ret);
    if (ret < 0)
    {
        msg.valid = -1;
        return msg;
    }

    ret = process_fmtd_hdr(hdr, &msg);

    if (ret < 0)
    {
        msg.valid = -1;
        return msg;
    }

    msg_contents = ALLOC(char, msg.msg_contents_len + 1);
    
    if (msg_contents == NULL)
    {
        msg.valid = -1;
        return msg;
    }

    ret = recv_msg_socket(bluetooth_fd, msg_contents, msg.msg_contents_len);

    if (ret < 0)
    {
        free(msg_contents);
        msg.valid = -1;
        return msg;
    }

    msg.msg_contents = msg_contents;

    msg.valid = 0;
    return msg;
}

int send_fmtd_msg(int bluetooth_fd, char type, char * contents)
{
 
    char * msg_to_send = NULL;
    char len_buff[5] = {0};
    int msg_len = strlen(contents);
    int ret = 0;
    if (msg_len >= 10000)
    {
        ERROR("Max size of message contents: 9999\n");
    } 
    else
    {
        msg_to_send = ALLOC(char, msg_len + 1 + 4 + 1); // type, 4-character length field, 1 NT char
        
        if (msg_to_send == NULL)
        {
            ret = -1;
        }
        else
        {
            msg_to_send[0] = type;
	    int i = 1;
	    for (i = 1; i <=4; i++)
	    {
		msg_to_send[i] = ' ';
            }
	    
            sprintf(len_buff, "%d", msg_len);
	    strncpy(&msg_to_send[1], len_buff, strlen(len_buff));
            sprintf(&msg_to_send[5], "%s", contents);
	    printf("SENDING CONTENTS: %s\n", contents);
	    printf("FULL MESSAGE: %s %d\n", msg_to_send, strlen(msg_to_send));
            if (send_msg_socket(bluetooth_fd, msg_to_send, strlen(msg_to_send)) < 0)
            {
                ret = -1;
            }
            free(msg_to_send);
        }
    }
    return ret;
}

/*
 *  Sends a JSON message to the controller.
 */ 
char * send_msg_to_controller(int controller_fd, char * contents)
{
    controller_msg_hdr_t hdr;

    send_msg_hdr(controller_fd, CTRL_MSG_JSON, strlen(contents) + 1);
    send_msg_string(controller_fd, contents);

	recv_msg_hdr(controller_fd, &hdr);	  
      
    DEBUG("Received new message of type %s\n", msg_type_to_string(hdr.type));
	     
    if (hdr.type != CTRL_MSG_JSON)
    {
        ERROR("Expected JSON reply from controller\n");
        return NULL;
    }       	    

	char * return_msg = ALLOC(char, hdr.len);
		             
	recv_msg_socket(controller_fd, return_msg, hdr.len);

    printf("Received response: %s\n", return_msg);
    
    return return_msg;
}

int setup_bluetooth_socket(void)
{
    // https://people.csail.mit.edu/albert/bluez-intro/x502.html
    struct sockaddr_rc loc_addr = { 0 };
    int sockfd;
    int ret = 0;

    // allocate socket
    sockfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (sockfd < 0)
    {
        return -1;
    }

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) BLUETOOTH_CHANNEL;
    ret = bind(sockfd, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    if (ret < 0)
    {
        close(sockfd);
        return -1;
    }

    // put socket into listening mode

    ret = listen(sockfd, 1);
    if (ret < 0)
    {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int handle_bluetooth_client(int bluetooth_fd, int controller_fd)
{
    int msg_exit = 0;
    struct bluetooth_msg msg;
    char * controller_reply = NULL;

    while (!msg_exit)
    {
//	send_fmtd_msg(bluetooth_fd, 'C', "Hello");
	//sleep(100);

	printf("Entering recv_fmtd_msg()\n");
        msg = recv_fmtd_msg(bluetooth_fd);

	if (msg.valid == -1)
	{
		printf("Invalid message; now exiting\n");
		break;
	}

	 printf("Message received: %s\n", msg.msg_contents); 

        switch (msg.type)
        {
            case 'C': controller_reply = send_msg_to_controller(controller_fd, msg.msg_contents);
                      if (controller_reply != NULL)
                      {
                          send_fmtd_msg(bluetooth_fd, 'C', controller_reply);
                      }
                      break;
            case 'D': printf("Database command received!\n");
	int arraysize;
	int i;
	int j;
	cJSON* message;
	cJSON *table = cJSON_CreateArray();
	cJSON *msgTable;

	if(sych_databases(1, table)) return 1;
	arraysize = cJSON_GetArraySize(table);
	for(i = 0; i < arraysize; i += MSG_SIZE )
	{
		msgTable = cJSON_CreateArray();
		message = cJSON_CreateObject();
		if(arraysize - i > MSG_SIZE)
		{
			for(j = 0; j < MSG_SIZE; j++)
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
                send_fmtd_msg(bluetooth_fd, 'D', cJSON_Print(message));
		cJSON_Delete(message);
	}
                      break;
            default:  break;
        }
    }
    return 0;
}

int main(void) 
{
    int main_exit = 0; 
    int bluetooth_server_fd = 0;
    int bluetooth_client_fd = 0;
    int controller_fd = 0;

    controller_fd = init_client(CONTROLLER_SOCKET_PATH);

    if (controller_fd < 0)
    {
        fprintf(stderr, "Failed to initiate communication with controller\n");
    }

    bluetooth_server_fd = setup_bluetooth_socket();

    if (bluetooth_server_fd < 0)  
    {
        fprintf(stderr, "Failed to setup bluetooth socket\n");
        exit(1);
    }

    printf("Now waiting..\n");
    while (!main_exit) { 
	printf("Wait..\n");
        bluetooth_client_fd = accept(bluetooth_server_fd, NULL, NULL);

	printf("Connection arrived!\n");
        if (bluetooth_client_fd < 0)    
        {

        }

        if (handle_bluetooth_client(bluetooth_client_fd, controller_fd) < 0)
        {

        }

        if (close(bluetooth_client_fd) < 0)
        {

        }
    }

    close(controller_fd);
    close(bluetooth_server_fd);

    return 0;
}
