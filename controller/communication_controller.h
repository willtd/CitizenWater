/*
 * communication_controller.h
 *
 *  Provides an API for communication between the controller and client 
 *  processes.
 */


#ifndef COMMUNICATION_CONTROLLER_H_

#define COMMUNICATION_CONTROLLER_H_

#define MAX_VAR_LEN 128
#define MAX_JSON_MSG_LEN 4096

typedef enum controller_msg_type_t 
{
    CTRL_MSG_ERROR = -1,    /* Client requests communication to be terminated, with failure */

    CTRL_MSG_JSON,          /* Followed by string containing JSON-formatted message. 
                             * A JSON message from client->controller is followed by a JSON message
                             * from controller to client.
                             */

    CTRL_MSG_REPLY,         /* Controller replies to client with a simple return value integer */

    CTRL_MSG_EXIT           /* Client requests communication to be terminated, no failure */
} controller_msg_type_t;

typedef struct controller_msg_reply_t 
{
    int return_value;     /* -1 on error, 0 on success, > 0 on success and indicates length of
                           *   string to follow 
                           */
} controller_msg_reply_t;

typedef struct controller_msg_hdr_t 
{
    controller_msg_type_t type;
    int                   len; /* Length of message to follow (in bytes), if any */
} controller_msg_hdr_t;

/*
 *  Returns the message type as a human-readable string.
 */
char * msg_type_to_string(controller_msg_type_t msg_type);

/*
 *   Returns -1 on ERROR and 0 on SUCCESS
 */ 
int recv_msg_hdr(int clientfd, controller_msg_hdr_t * msg_hdr);

/* 
 *  Returns -1 on ERROR and 0 on SUCCESS
 */
int send_msg_hdr(int sockfd, controller_msg_type_t msg_type, int msg_len);

/*
 *  Sends the string str. 'str' should be a nullterminated string
 *  Returns -1 on ERROR and 0 on SUCCESS
 */
int send_msg_string(int sockfd, char * str);

/*
 *  Receives a string of size msg_len. Returns -1 on ERROR and 0 on SUCCESS
 */
int recv_msg_string(int sockfd, char * str, int msg_len);

/*
 * Returns -1 on ERROR and 0 on SUCCESS
 */
int send_msg_reply(int sockfd, int ret);

/* 
 * Returns -1 on ERROR and the return value of the message of the reply on SUCCESS
 */
int recv_msg_reply(int sockfd);


#endif
