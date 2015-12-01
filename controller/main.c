/* 
 * main.c
 *
 * This module is responsible for decoding and acting on messages received
 * from the outside world. It does not communicate directly with the outside
 * world, rather, it communicates indirectly via an auxiliary process (the client)
 * which is responsible for communicating with the outside world. For example, 
 * this auxiliary process may be in charge of handling a Bluetooth connection
 * between the microcontroller and the outside world.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../shared/log.h"
#include "../shared/sockets.h"
#include "../shared/util.h"
#include "../shared/macros.h"

#include "communication_controller.h"
#include "handler.h"
#include "options.h"

/*
 *  Never returns. 
 */
void main_loop(int sockfd) {
    while (1) 
    {   
	    int clientfd = wait_for_connection(sockfd);
        
        INFO("main_loop(): New client. clientfd: %d\n", clientfd);
  
        if (clientfd == -1)
        {
            ERROR("main_loop(): error occurred in wait_for_connection");
            continue;
        }

	    int cont = 1;
	    while (cont)
	    {
            INFO("main_loop(): Waiting for message...\n");

            controller_msg_hdr_t hdr;

	        if (recv_msg_hdr(clientfd, &hdr) == -1)
            {
                ERROR("main_loop(): Error occurred in recv_msg_hdr()\n");
                cont = 0;
                break;
            }
	        
	        INFO("main_loop(): Servicing message of type %s\n", msg_type_to_string(hdr.type));
	        	    
	        switch (hdr.type) 
	        {
		        case CTRL_MSG_JSON: 
                {
                    if (hdr.len > MAX_JSON_MSG_LEN)
                    {
                        WARN("main_loop(): JSON message may be excessively large.\n");
                    }
                    
		            char * json_msg = ALLOC(char, hdr.len);

		            if (recv_msg_string(clientfd, json_msg, hdr.len) == -1)
                    {
                        ERROR("main_loop(): Error occurred in recv_msg_string()\n");
                        free(json_msg);
                        cont = 0;
                        break;
                    }

                    json_msg[hdr.len - 1] = '\0';

		            INFO("main_loop(): JSON Message received. Contents:\n%s\n", json_msg);

		            char * return_msg = handle_msg(json_msg);
                 
                    INFO("main_loop(): Replying to client with message: \n%s\n", return_msg);

                    if (send_msg_hdr(clientfd, CTRL_MSG_JSON, strlen(return_msg) + 1) != -1)
                    {
                        if (send_msg_string(clientfd, return_msg) != -1)
                        {
                            
                        }
                        else
                        {
                            ERROR("send_msg_string() failed, terminating communication with client...\n");
                            cont = 0;
                        }
                    }
                    else
                    {
                        ERROR("send_msg_hdr() failed, terminating communication with client...\n");
                        cont = 0;
                    }

                    free(return_msg);
                    free(json_msg);

		            break;
		        }
		        case CTRL_MSG_REPLY: 
                {
                    WARN("main_loop(): Received CTRL_MSG_REPLY... ignoring message\n");
                    break;
                }
		        case CTRL_MSG_EXIT: 
                {
		            INFO("main_loop(): Received CTRL_MSG_EXIT\n");
		            cont = 0;
		            break;
                }
		        case CTRL_MSG_ERROR: 
                {
		            ERROR("main_loop(): Received CTRL_MSG_ERROR\n");     
		            cont = 0;
		            break;
                }
    		    default: 
                {
		            ERROR("main_loop(): Message of unknown type was received. Terminating communication..\n");
		            cont = 0;
		            break;
                }
	        }
	    }

	    INFO("main_loop(): Breaking connection with client...\n");

	    close_socket(clientfd); 
    }
}

int main(int argc, char * argv[]) {
    int sockfd = -1;
    controller_options_t opts;

    // Parse and apply command line options
    parse_options(argc, argv, &opts);
    print_options(&opts);

    DEBUG("main(): Finished parsing options...\n");

    set_log_level(opts.log_level);
    enable_syslog("citwater-controller", 1);

    // Remove any previous instance. Removing arbitrary file is a bit dangerous
    remove_file(opts.socket_file);
    sockfd = init_server(opts.socket_file);

    if (sockfd == -1)
    {
	    PERROR("main() -- init_server()");
	    exit(1);
    }

    if (daemon(0, 0) == -1)
    {
        PERROR("main() -- daemon()"); 
        exit(1);
    }

    if (atlsci_init() < 0)
    {
        ERROR("main() -- atlsci_init()");
        exit(1);
    }

    main_loop(sockfd);

    atlsci_deinit();
    disable_syslog();
    return 0;
}
