
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../shared/log.h"
#include "../shared/macros.h"
#include "../shared/sockets.h"

#include "mraa-shared-defs.h"
#include "mraa.h"

// For atlsci sensors, a 300ms wait is typically needed between sending a command
// and reading a response. Set to 2 seconds because millisecond resolution is difficult
// to achieve.
#define TX_WAIT_TIME 2

static int set_address(mraa_i2c_context i2c_context, int addr)
{
	mraa_result_t mraa_result = mraa_i2c_address(i2c_context, addr);

    if (mraa_result != MRAA_SUCCESS)
    {
        DEBUG("set_address(): mraa_i2c_address() failure, error code: %d\n", mraa_result);
        mraa_result_print(mraa_result);
    }

    return mraa_result;
}

/**
 *  Transmits a string. msg should be null-terminating, but the null byte
 *  is not transmitted. This function should be called after calling
 *  set_address(). Delays for TX_WAIT_TIME seconds after sending message.
 */
static int send_string(mraa_i2c_context i2c_context, char * msg)
{
    DEBUG("Sending string %s of length %d\n", msg, strlen(msg));

    int ret = mraa_i2c_write(i2c_context, (uint8_t *) msg, strlen(msg));
    sleep(TX_WAIT_TIME);
    return ret;
}

static int read_string(mraa_i2c_context i2c_context, char * buff, int buff_len)
{
    DEBUG("Attempting to read string of length %d\n", buff_len);
	return mraa_i2c_read(i2c_context, buff, buff_len);
}

void main_loop(int sockfd, mraa_i2c_context i2c_context)
{
	int clientfd;

	while (1) 
	{
	    struct mraa_cmd cmd = {0};

		clientfd = wait_for_connection(sockfd);
		
		if (clientfd < 0)
		{
			ERROR("wait_for_connection() failed\n");
            continue;
		}
		
		if (recv_msg_socket(clientfd, &cmd, sizeof(cmd)) != sizeof(cmd))
		{
		    ERROR("recv_msg_socket() failed\n");	
		}
		else
        {
            DEBUG("buffer address: %d\n, buffer contents: %s\n", cmd.addr, cmd.buffer);

		    /* Setup address */
		    if (set_address(i2c_context, cmd.addr) < 0)
		    {
			    ERROR("set_address() failed\n");
                cmd.retval = -1;
		    }
		    else
            {
                cmd.retval = -1;
		        /* Send data */
                
		        if (send_string(i2c_context, cmd.buffer) < 0)
		        {
			        ERROR("send_string() failed\n");
                    cmd.retval = -1;
		        }
		        
		        /* Read reply from MRAA */
                
		        if (read_string(i2c_context, cmd.reply, sizeof(cmd.reply)) < 0)
		        {
		            ERROR("read_string() failed\n");
                    cmd.retval = -1;
		        }
                
		    }

		    /* Send reply */
		    if (send_msg_socket(clientfd, &cmd, sizeof(cmd)) != sizeof(cmd))
		    {
			    ERROR("send_msg_socket() failed\n");
		    }
        }

        close_socket(clientfd);
	}
}

int server_setup(void)
{
	int sockfd = init_server(MRAA_SERVER);
	if (sockfd == -1)
	{
		PERROR("server_setup(): init_server() failed\n");
		exit(1);
	}
	
	return sockfd;
}

mraa_i2c_context mraa_setup(void)
{
    mraa_i2c_context i2c_context;

	i2c_context = mraa_i2c_init_raw(6);

    if (i2c_context == NULL)
    {
        DEBUG("i2c_init(): mraa_i2c_init_raw() failure\n");
    }

	
	return i2c_context;
}

int main(void)
{
	int sockfd = server_setup();
	mraa_i2c_context context = mraa_setup();
    enable_syslog("mraa-handler", 1);

	main_loop(sockfd, context);
    disable_syslog();
    return 0;
}
