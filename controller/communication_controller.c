/* 
 * communication_controller.c
 *
 */

#include <stdlib.h>

#include "../shared/log.h"
#include "../shared/sockets.h"

#include "communication_controller.h"

#define ALL_BYTES_SENT(x, y) (((x) == (y)) ? 0 : -1)

char * msg_type_to_string(controller_msg_type_t msg_type) 
{
    switch (msg_type)
    {
	    case CTRL_MSG_ERROR:             return "ERROR";
	    case CTRL_MSG_JSON:              return "JSON";
        case CTRL_MSG_REPLY:             return "REPLY";
	    case CTRL_MSG_EXIT:              return "EXIT";
	    default:                         return "Unknown";
    }
}


int recv_msg_hdr(int clientfd, controller_msg_hdr_t * msg_hdr) 
{
    int ret = recv_msg_socket(clientfd, msg_hdr , sizeof(*msg_hdr));

    return (ret == -1) ? -1 : 0;
}


int send_msg_hdr(int sockfd, controller_msg_type_t msg_type, int msg_len)
{
     controller_msg_hdr_t msg_hdr = {0};    
     
     msg_hdr.type = msg_type;
     msg_hdr.len  = msg_len;
     
     return ALL_BYTES_SENT(send_msg_socket(sockfd, &msg_hdr, sizeof(msg_hdr)), sizeof(msg_hdr));
}



int send_msg_reply(int sockfd, int ret)
{
    controller_msg_reply_t msg = {ret};
        
    return ALL_BYTES_SENT(send_msg_socket(sockfd, &msg, sizeof(msg)), sizeof(msg));
}


int recv_msg_reply(int sockfd) 
{
    controller_msg_reply_t msg;
    int ret = recv_msg_socket(sockfd, &msg, sizeof(msg));
    return (ret != sizeof(msg)) ? -1 : msg.return_value;
}


int recv_msg_string(int sockfd, char * str, int msg_len)
{
    return (recv_msg_socket(sockfd, str, msg_len) != msg_len ? -1 : 0);
}


int send_msg_string(int sockfd, char * str) 
{
    return ALL_BYTES_SENT(send_msg_socket(sockfd, str, strlen(str) + 1), strlen(str) + 1);
}


