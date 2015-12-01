/*
 *   Sample client for controller
 *
 *   Updates:
 *    6/8/15: Client now can send multiple JSON messages. 
 *            The contents of JSON messages are now paths rather than JSON
 *            strings.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../shared/sockets.h"
#include "../shared/log.h"
#include "../shared/macros.h"
#include "../shared/util.h"

#include "../controller/communication_controller.h"

#define CONTROLLER_SOCKET_PATH "/tmp/water-controller-socket"

/*
 *  Communication to the controller:

 1. Client initializes communication by connecting to the controller through
    init_client(socket_path);
 2. Client writes its message to a local file. Client closes file when
    finished.
    example of path: /tmp/citwater-bluetooth
 3. Client sends the path name over the socket using
    send_msg_hdr followed by send_msg_string
 4. Controller receives message, opens file, and decodes
 5. Controller writes its reply message to a path and closes both files
 6. Controller sends the path to the client. The client can open the path
    to see the response
 *
 */

// The sockfd is used for sending a file name to the controller process.
int sockfd = -1;

/*
// The comm_file is the file which the controller will read from.
FILE * comm_file = NULL;

void generate_file_name(char * prefix)
{
    // Of the form /tmp/citwater-prefix
}
*/

/*
 *  Prints relevant options to stdout.
 */
void print_options()
{
    printf("Options: \n");
    printf("         s: Send JSON message\n");
    printf("         x: Send EXIT message and exit program\n");
}

/*
 *  Sends a JSON message to the controller.
 *  The user should input a path, which will be the contents of the JSON message.
 */ 
int client_json_handler()
{
    char path[PATH_SIZE] = {0};

    printf("Enter a file name: ");
    scanf("%s", path);

    printf("Path: %s\n", path);

    char * file_contents =  get_file_as_string(path);

    printf("File contents: %s\n", file_contents);

    if (file_contents != NULL)
    {
        // Sending path
        send_msg_hdr(sockfd, CTRL_MSG_JSON, strlen(file_contents) + 1);
        send_msg_string(sockfd, file_contents);

        // Receive reply
        controller_msg_hdr_t hdr;
	    recv_msg_hdr(sockfd, &hdr);	        
	    DEBUG("Received new message of type %s\n", msg_type_to_string(hdr.type));
	     
        if (hdr.type != CTRL_MSG_JSON)
        {
            ERROR("Expected JSON reply\n");
            return -1;
        }       	    

	    char * return_msg = ALLOC(char, hdr.len);
		             
	    recv_msg_socket(sockfd, return_msg, hdr.len);

        printf("Received response: %s\n", return_msg);

        // Open path, read contents, act appropriately
    }
    return 0;
}

int main(void) {
    sockfd = init_client(CONTROLLER_SOCKET_PATH);
    
    if (sockfd == -1)
    {
        printf("Failed to open socket..\n");
    }
    else
    {
        printf("Opened socket successfully\n");
    }

    int exit = 0;
    int print_opts = 1;
    
    while (!exit)
    {   
        if (print_opts)
            print_options();

        
        char c = getchar();
        //DEBUG("char %d\n", c);
        char buff[80] = {0}; 
        get_current_time(buff, 80);

        switch (c)
        {
            case 's': 
                printf("Received command. Timestamp: %s\n", buff);
                printf("Sending JSON command...\n");
                client_json_handler();
                print_opts = 1;
                break;
            case 'x': 
                printf("Received command. Timestamp: %s\n", buff);
                printf("Sending exit command...\n");
                send_msg_hdr(sockfd, CTRL_MSG_EXIT, 0);    
                print_opts = 1;
                exit = 1;

                break;
            case ' ':
            case '\n':
            case '\t':
                print_opts = 0;
                break;
            default:
                printf("Unrecognized character %c\n", c);
                print_opts = 1;
                break;
        }
    }

    return 0;
}
